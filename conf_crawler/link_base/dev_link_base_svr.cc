/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/dev_link_base_svr.cc
 * @namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-21
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

#include "ganji/crawler/conf_crawler/link_base/DevLinkBaseService.h"
#include "ganji/crawler/conf_crawler/link_base/conf_crawler_types.h"
#include "ganji/crawler/conf_crawler/link_base/link_config.h"
#include "ganji/crawler/conf_crawler/link_base/dev_link_base.h"
#include "ganji/util/log/thread_fast_log.h"
#include "ganji/util/thread/thread.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"

using std::string;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace ganji::util::log::ThreadFastLog;
namespace FastLog = ganji::util::log::ThreadFastLog;
namespace thread = ganji::util::thread;
using thread::Thread;
namespace Sleep = ::ganji::util::thread::Sleep;
using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::crawler::conf_crawler::link_base::LinkConfig;
using ganji::crawler::conf_crawler::link_base::DevLinkBase;
using ganji::crawler::conf_crawler::LongConnHandler;

class DevLinkBaseServiceHandler : virtual public DevLinkBaseServiceIf {
 public:
  DevLinkBaseServiceHandler()
    : p_config_(NULL) {
  }

  int Init(LinkConfig *p_config) {
    p_config_ = p_config;
    if (dev_link_base_.Init(p_config_) < 0) {
      WriteLog(kLogFatal, "link base Init failed");
      return -1;
    }

    return 0;
  }

  void dev_url(MatchedResultItem &_return, const DevUrlItem &dev_url_item) {
    dev_link_base_.DevUrl(dev_url_item, &_return);
  }

  int extract_update_conf() {
    int ret = dev_link_base_.ExtractUpdateConf();
    return ret;
  }

  int32_t test_active() {
    return 0;
  }

 private:
  LinkConfig *p_config_;
  DevLinkBase dev_link_base_;
};

int main(int argc, char **argv) {
  static FastLogStat log_st = {kLogAll, kLogFatal, kLogSizeSplit};
  FastLog::OpenLog("./log", "linkbase", 2048, &log_st);

  LinkConfig config;
  config.LoadConfig("conf.txt");
  config.PrintConfig();

  int nb_thread_count = config.GetNbThreadCount();
  int port = config.GetLinkBasePort();

  boost::shared_ptr<DevLinkBaseServiceHandler> handler(new DevLinkBaseServiceHandler());
  if (handler->Init(&config) < 0)
    return 1;

  boost::shared_ptr<TProcessor> processor(new DevLinkBaseServiceProcessor(handler));
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  boost::shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager(nb_thread_count);
  boost::shared_ptr<PosixThreadFactory> thread_factory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  thread_manager->threadFactory(thread_factory);
  thread_manager->start();

  TNonblockingServer server(processor, protocolFactory, port, thread_manager);

  FastLog::WriteLog(kLogNotice, "--------------begin serving!-------------");

  server.serve();

  FastLog::WriteLog(kLogNotice, "--------------serving stopped!-------------");
  FastLog::CloseLog(0);

  return 0;
}

