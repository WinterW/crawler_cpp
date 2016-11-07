/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/extractor/conf_extractor_svr.cc
 * @namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-25
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <signal.h>

#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TNonblockingServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TTransportException.h>
#include <concurrency/PosixThreadFactory.h>

#include <string>
#include <vector>

#include "util/thread/thread.h"
#include "util/thread/sleep.h"
#include "conf_crawler/common/long_short_conn.h"
#include "conf_crawler/extractor/StaticLinkBaseService.h"
#include "conf_crawler/extractor/ExtractorService.h"
#include "conf_crawler/extractor/conf_crawler_types.h"

#include "conf_crawler/extractor/plain_extractor.h"
#include "conf_crawler/extractor/css_extractor.h"
#include "conf_crawler/extractor/relay_extractor.h"
#include "conf_crawler/extractor/extractor_config.h"
#include "util/log/thread_fast_log.h"

using std::string;
using std::vector;
using boost::shared_ptr;

using apache::thrift::TProcessor;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;
using apache::thrift::server::TNonblockingServer;
using apache::thrift::TException;
using apache::thrift::transport::TTransportException;

using ganji::util::thread::Thread;
using ganji::util::thread::Mutex;
namespace Sleep = ganji::util::thread::Sleep;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogNone;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;
using FastLog::FastLogStat;
using FastLog::kLogAll;
using FastLog::kLogSizeSplit;
using FastLog::CloseLog;

using ganji::crawler::conf_crawler::LongShortConn;
using ganji::crawler::conf_crawler::extractor::PlainExtractor;
using ganji::crawler::conf_crawler::extractor::CssExtractor;
using ganji::crawler::conf_crawler::extractor::RelayExtractor;
using ganji::crawler::conf_crawler::extractor::ExtractorConfig;

class ExtractorServiceHandler: virtual public ExtractorServiceIf {
 public:
  ExtractorServiceHandler() {
  }

  int Init(ExtractorConfig *p_config) {
    p_config_ = p_config;

    int socket_timeout = p_config_->GetSocketTimeout();
    int persist_count = p_config_->GetPersistCount();
    link_base_conn_.Init(p_config->GetLinkBaseHost(),
                         p_config->GetLinkBasePort(),
                         socket_timeout,
                         persist_count);

    bool is_get_task = p_config_->IsGetTask();
    if (is_get_task) {
      p_get_task_thread_ = Thread::CreateThread(GetTaskThread, this);
      p_get_task_thread_->ResumeThread();
    }

    if (plain_extractor_.Init(p_config) < 0) {
      WriteLog(kLogFatal, "plain_extractor Init failed");
      return -1;
    }
    if (css_extractor_.Init(p_config) < 0) {
      WriteLog(kLogFatal, "css_extractor Init failed");
      return -1;
    }
    if (relay_extractor_.Init(p_config) < 0) {
      WriteLog(kLogFatal, "relay_extractor Init failed");
      return -1;
    }
    return 0;
  }

  /// load template based on template_type
  int load_template(const string &url_template, const TemplateType::type template_type) {
    int ret = 0;
    string err_info;
    if (template_type == TemplateType::type::PLAIN_HTML_TYPE) {
      ret = plain_extractor_.LoadTemplate(url_template, &err_info);
    } else if (template_type == TemplateType::type::CSS_SELECTOR_TYPE) {
      ret = css_extractor_.LoadTemplate(url_template, &err_info);
    } else {
      WriteLog(kLogFatal, "load_template[%s], invalid type:%d", url_template.c_str(), template_type);
      ret = -1;
    }
    if (ret == 0)
      WriteLog(kLogNotice, "load_template[%s], type:%d OK", url_template.c_str(), template_type);
    else
      WriteLog(kLogFatal, "load_template[%s], type:%d failed", url_template.c_str(), template_type);
    return ret;
  }

  void unload_template(const string &url_template, const TemplateType::type template_type) {
    return;
  }

  /// extract based on template_type
  void extract_sync(MatchedResultItem &_return, const ExtractItem &extract_item) {
    TemplateType::type template_type = extract_item.template_type;
    int ret = 0;
    const string &url = extract_item.url;
    const string &url_template = extract_item.url_template;
    int depth = extract_item.depth;
    if (template_type == TemplateType::type::PLAIN_HTML_TYPE) {
      ret = plain_extractor_.ExtractWrapper(extract_item, &_return);
    } else if (template_type == TemplateType::type::CSS_SELECTOR_TYPE) {
      ret = css_extractor_.ExtractWrapper(extract_item, &_return);
    } else if (template_type == TemplateType::type::RELAY_TYPE) {
      ret = relay_extractor_.ExtractWrapper(extract_item, &_return);
    } else {
      WriteLog(kLogNotice, "extract_sync[%s] template[%s] depth[%d], invalid type:%d",
               url.c_str(),
               url_template.c_str(),
               depth,
               template_type);
      ret = -1;
    }

    if (ret == 0)
      _return.is_ok = true;
  }

 private:
  /// get task from link base
  static void *GetTaskThread(void *arg);
  int GetTaskThreadFunc();

 private:
  ExtractorConfig *p_config_;

  LongShortConn<StaticLinkBaseServiceClient> link_base_conn_;     ///< connection with link base
  Thread *p_get_task_thread_;  ///< get task thread

  PlainExtractor plain_extractor_;    ///< plain extractor
  CssExtractor css_extractor_;        ///< css extractor
  RelayExtractor relay_extractor_;    ///< relay extractor
};

void *ExtractorServiceHandler::GetTaskThread(void *arg) {
  ExtractorServiceHandler *p_handler = reinterpret_cast<ExtractorServiceHandler *>(arg);
  int time_slice = p_handler->p_config_->GetTimeSlice();

  while (true) {
    Sleep::DoSleep(time_slice);
    p_handler->GetTaskThreadFunc();
  }

  return NULL;
}

int ExtractorServiceHandler::GetTaskThreadFunc() {
  int ret = 0;
  vector<ExtractItem> task_list;
  link_base_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (link_base_conn_.NeedReset()) {
      bool is_ok = link_base_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetReqRetryTimes(); i++) {
      try {
        link_base_conn_.Client()->get_extract_task(task_list);
        link_base_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(const TTransportException &te) {
        WriteLog(kLogNotice, "get_extract_task(), Exception:%s [%d]", te.what(), te.getType());
      } catch(const TException &e) {
        WriteLog(kLogNotice, "get_extract_task(), Exception:%s", e.what());
      }
      ret = -1;
      bool is_ok = link_base_conn_.Reset();
      if (!is_ok) {
        WriteLog(kLogNotice, "get_extract_task() failed, times:%d Reset failed", i);
        break;
      } else {
        WriteLog(kLogNotice, "get_extract_task() failed, times:%d Reset OK", i);
      }
    }
  } while (0);
  link_base_conn_.Unlock();

  for (vector<ExtractItem>::iterator it = task_list.begin();
      it != task_list.end(); ++it) {
    TemplateType::type template_type = it->template_type;
    if (template_type == TemplateType::type::PLAIN_HTML_TYPE) {
      plain_extractor_.PushIntoExtractQueue(*it);
      WriteLog(kLogNotice, "GetTask[%s] template[%s] depth[%d] PLAIN_HTML",
               it->url.c_str(),
               it->url_template.c_str(),
               it->depth);
    } else if (template_type == TemplateType::type::CSS_SELECTOR_TYPE) {
      css_extractor_.PushIntoExtractQueue(*it);
      WriteLog(kLogNotice, "GetTask[%s] template[%s] depth[%d] CSS_SELECTOR",
               it->url.c_str(),
               it->url_template.c_str(),
               it->depth);
    } else if (template_type == TemplateType::type::RELAY_TYPE) {
      relay_extractor_.PushIntoExtractQueue(*it);
      WriteLog(kLogNotice, "GetTask[%s] template[%s] depth[%d] RELAY",
               it->url.c_str(),
               it->url_template.c_str(),
               it->depth);
    } else {
      WriteLog(kLogNotice, "GetTask[%s] template[%s] depth[%d], invalid type:%d",
               it->url.c_str(),
               it->url_template.c_str(),
               it->depth,
               template_type);
    }
  }

  return 0;
}

void SigUser1(int sig_no) {
  exit(0);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:%s conf_path log_path\n", argv[0]);
    return 1;
  }

  /// 处理信号USR1，用于测试性能
  signal(SIGUSR1, SigUser1);

  string conf_path = argv[1];
  string log_path = argv[2];

  static FastLogStat log_st = {kLogAll, kLogNone, kLogSizeSplit};
  OpenLog(log_path.c_str(), "extractor", 2048, &log_st);

  ExtractorConfig config;
  int ret = config.LoadConfig(conf_path);
  if (ret < 0) {
    printf("LoadConfig failed");
    return 1;
  }
  config.PrintConfig();

  int nb_thread_count = config.GetNbThreadCount();
  int port = config.GetExtractorPort();

  shared_ptr<ExtractorServiceHandler> handler(new ExtractorServiceHandler());
  if (handler->Init(&config) < 0)
    return 1;

  shared_ptr<TProcessor> processor(new ExtractorServiceProcessor(handler));
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager(nb_thread_count);
  shared_ptr<PosixThreadFactory> thread_factory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  thread_manager->threadFactory(thread_factory);
  thread_manager->start();

  TNonblockingServer server(processor, protocolFactory, port, thread_manager);

  WriteLog(kLogNotice, "--------------begin serving!-------------");

  server.serve();

  WriteLog(kLogNotice, "--------------serving stopped!-------------");
  CloseLog(0);

  return 0;
}

