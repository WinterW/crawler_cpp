/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/static_dedup_svr.cc
 * @namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-01
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
#include <utility>

#include "conf_crawler/url_dedup/DedupService.h"
#include "conf_crawler/url_dedup/conf_crawler_types.h"
#include "conf_crawler/url_dedup/dedup_config.h"
#include "conf_crawler/url_dedup/static_dedup.h"
#include "util/log/thread_fast_log.h"
#include "util/thread/thread.h"
#include "util/thread/sleep.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"

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
using FastLog::kLogSizeSplit;
using FastLog::CloseLog;

using ganji::crawler::conf_crawler::dedup::DedupConfig;
using ganji::crawler::conf_crawler::dedup::StaticDedup;

class DedupServiceHandler : virtual public DedupServiceIf {
 public:
  DedupServiceHandler() {
  }

  int Init(DedupConfig *p_config) {
    if (dedup_.Init(p_config) < 0)
      return -1;
    return 0;
  }

  void is_exists(DedupExistItem &_return, const std::string &url) {
    bool ret = dedup_.IsExists(url);
    _return.is_exists = ret;
  }

  bool insert(const std::string &url) {
    return dedup_.Insert(url);
  }

  bool test_exists_and_insert(const std::string& url) {
    bool is_exists = true;
    dedup_.TestExistsAndInsert(url, &is_exists);
    return is_exists;
  }

  bool remove(const std::string &url) {
    return dedup_.Remove(url);
  }

  void info(std::string &info) {
  }

  int set_bucket_count(int bucket_count) {
    dedup_.Rehash(bucket_count);
    return 0;
  }

  int batch_remove(const std::string &url_pattern) {
    int ret = dedup_.BatchRemove(url_pattern);
    return ret;
  }

 private:
  StaticDedup dedup_;
};

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:%s conf_path log_path\n", argv[0]);
    return 1;
  }

  string conf_path = argv[1];
  string log_path = argv[2];

  static FastLogStat log_st = {kLogAll, kLogFatal, kLogSizeSplit};
  OpenLog(log_path.c_str(), "staticdedup", 2048, &log_st);

  DedupConfig config;
  config.LoadConfig(conf_path);
  config.PrintConfig();

  int nb_thread_count = config.GetNbThreadCount();
  int port = config.GetDedupPort();

  shared_ptr<DedupServiceHandler> handler(new DedupServiceHandler());
  if (handler->Init(&config) < 0)
    return 1;

  shared_ptr<TProcessor> processor(new DedupServiceProcessor(handler));
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

