/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/dc/dc_svr.cc
 * @namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <stdlib.h>
#include <vector>

#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TNonblockingServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <concurrency/PosixThreadFactory.h>

#include "conf_crawler/dc/DCService.h"
#include "dc_config.h"
#include "dc.h"
#include "conf_crawler/dc/conf_crawler_types.h"
#include "util/log/thread_fast_log.h"

using apache::thrift::TProcessor;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;
using apache::thrift::server::TNonblockingServer;

using std::string;
using std::vector;
using boost::shared_ptr;

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

namespace Sleep = ::ganji::util::thread::Sleep;
using ganji::util::thread::Thread;
using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::crawler::conf_crawler::dc::DcConfig;
using ganji::crawler::conf_crawler::dc::DcManager;

class DCServiceHandler : virtual public DCServiceIf {
 public:
  DCServiceHandler() {
  }

  int Init(DcConfig *p_config) {
    if (dc_manager_.Init(p_config) < 0) {
      WriteLog(kLogFatal, "dc Init failed");
      return -1;
    }
    return 0;
  }

  /// get download task by downloader
  void get_download_task(vector<DownloadTask> &task_list, const DownloaderType::type downloader_type) {
    dc_manager_.OnGetTask(&task_list, downloader_type);
  }

  /// push download task, for debug
  void push_download_task(const DownloadTask &download_task) {
    dc_manager_.OnPushTask(download_task);
  }

  /// upload download task by downloader
  void upload_download_task(const DownloadedBodyItem &downloaded_body_item) {
    dc_manager_.OnUploadTask(downloaded_body_item);
  }

 private:
  DcManager dc_manager_;
};

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:%s conf_path log_path\n", argv[0]);
    return 1;
  }

  string conf_path = argv[1];
  string log_path = argv[2];

  static FastLogStat log_st = {kLogAll, kLogNone, kLogSizeSplit};
  FastLog::OpenLog(log_path.c_str(), "dc", 2048, &log_st);

  srandom(time(NULL));

  DcConfig config;
  config.LoadConfig(conf_path);
  config.PrintConfig();

  int nb_thread_count = config.GetNbThreadCount();
  int port = config.GetDcPort();

  shared_ptr<DCServiceHandler> handler(new DCServiceHandler());
  if (handler->Init(&config) < 0)
    return 1;

  shared_ptr<TProcessor> processor(new DCServiceProcessor(handler));
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager(nb_thread_count);
  shared_ptr<PosixThreadFactory> thread_factory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  thread_manager->threadFactory(thread_factory);
  thread_manager->start();

  TNonblockingServer server(processor, protocolFactory, port, thread_manager);

  FastLog::WriteLog(kLogNotice, "--------------begin serving!-------------");

  server.serve();

  FastLog::WriteLog(kLogNotice, "--------------serving stopped!-------------");
  FastLog::CloseLog(0);

  return 0;
}

