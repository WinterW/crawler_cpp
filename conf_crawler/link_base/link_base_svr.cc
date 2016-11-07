/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_base_svr.cc
 * @namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TNonblockingServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <concurrency/PosixThreadFactory.h>

#include <pthread.h>
#include <map>
#include <string>
#include <utility>

#include "ganji/crawler/conf_crawler/link_base/LinkBaseService.h"
#include "ganji/crawler/conf_crawler/link_base/conf_crawler_types.h"
#include "ganji/crawler/conf_crawler/link_base/link_config.h"
#include "ganji/crawler/conf_crawler/link_base/link_base.h"
#include "ganji/util/log/thread_fast_log.h"
#include "ganji/util/thread/thread.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"
#include "ganji/util/file/file.h"

using std::string;

using boost::shared_ptr;

using apache::thrift::TProcessor;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;
using apache::thrift::server::TNonblockingServer;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;
using FastLog::FastLogStat;
using FastLog::kLogAll;
using FastLog::kLogNone;
using FastLog::kLogSizeSplit;
using FastLog::CloseLog;

using ganji::crawler::conf_crawler::link_base::LinkConfig;
using ganji::crawler::conf_crawler::link_base::LinkBase;

class LinkBaseServiceHandler : virtual public LinkBaseServiceIf {
 public:
  LinkBaseServiceHandler()
    : p_config_(NULL) {
  }

  int Init(LinkConfig *p_config) {
    p_config_ = p_config;
    if (link_base_.Init(p_config_) < 0) {
      WriteLog(kLogFatal, "link base Init failed");
      return -1;
    }

    return 0;
  }

  /// load seed by id
  void load_seed_by_id(int id, bool is_add_link) {
    link_base_.LoadSeedById(id, is_add_link);
  }

  /// load seed by seed url
  void load_seed_by_url(const string &seed_url, bool is_add_link) {
    link_base_.LoadSeedByUrl(seed_url, is_add_link);
  }

  /// get download task by dc
  void get_download_task(std::vector<DownloadTask> & _return) {
    link_base_.OnGetDownloadTask(&_return);
  }

  /// get extract task by extractor
  void get_extract_task(std::vector<ExtractItem> & _return) {
    link_base_.OnGetExtractItem(&_return);
  }

  /// upload download task by dc
  void upload_download_task(const DownloadedBodyItem& downloaded_body_item) {
    link_base_.OnUploadBody(downloaded_body_item);
  }

  /// upload extract task by extractor
  void upload_extract_task(const ExtractItem& extract_item, const MatchedResultItem& matched_result_item) {
    link_base_.OnUploadExtract(extract_item, matched_result_item);
  }

 private:
  LinkConfig *p_config_;
  LinkBase link_base_;
};

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:%s conf_path log_path\n", argv[0]);
    return 1;
  }

  string conf_path = argv[1];
  string log_path = argv[2];

  static FastLogStat log_st = {kLogAll, kLogNone, kLogSizeSplit};
  OpenLog(log_path.c_str(), "linkbase", 2048, &log_st);

  LinkConfig config;
  config.LoadConfig(conf_path);
  config.PrintConfig();

  int nb_thread_count = config.GetNbThreadCount();
  int port = config.GetLinkBasePort();

  boost::shared_ptr<LinkBaseServiceHandler> handler(new LinkBaseServiceHandler());
  if (handler->Init(&config) < 0)
    return 1;

  boost::shared_ptr<TProcessor> processor(new LinkBaseServiceProcessor(handler));
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  boost::shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager(nb_thread_count);
  boost::shared_ptr<PosixThreadFactory> thread_factory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  thread_manager->threadFactory(thread_factory);
  thread_manager->start();

  TNonblockingServer server(processor, protocolFactory, port, thread_manager);

  WriteLog(kLogNotice, "--------------BEGIN SERVING!-------------");

  server.serve();

  WriteLog(kLogNotice, "--------------SERVING STOPPED!-------------");
  CloseLog(0);

  return 0;
}

