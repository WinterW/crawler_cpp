/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/common/long_conn.h
 * @namespace ganji::crawler::conf_crawler
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-26
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LONG_CONN_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LONG_CONN_H_

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

#include "util/log/thread_fast_log.h"
#include "util/thread/thread.h"
#include "util/thread/sleep.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"

namespace ganji { namespace crawler { namespace conf_crawler { 
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

/**
 * @class LongConnHandler
 * @brief 长连接handler基类
 */
template <typename T>
class LongConnHandler {
 public:
  explicit LongConnHandler()
    : p_sock_(NULL),
    p_trans_(NULL),
    p_prot_(NULL),
    p_client_(NULL) {
  }

  ~LongConnHandler() {
  }

  int Init(const string &peer_ip, int peer_port, int socket_timeout, int check_interval) {
    peer_ip_ = peer_ip;
    peer_port_ = peer_port;
    socket_timeout_ = socket_timeout;
    check_interval_ = check_interval;

    bool ret = Reconnect();
    if (!ret) {
      WriteLog(kLogFatal, "Init() connect() to %s:%d failed", peer_ip_.c_str(), peer_port_);
    } else {
      WriteLog(kLogFatal, "Init() connect() to %s:%d OK", peer_ip_.c_str(), peer_port_);
    }

    p_thread_ = thread::Thread::CreateThread(TestActiveFunc, this);
    p_thread_->ResumeThread();

    return 0;
  }

  void Clear() {
    if (p_client_ != NULL) {
      delete p_client_;
      p_client_ = NULL;
    }
    /// 删除client的时候，智能指针被自动销毁，不需要delete
    p_prot_ = NULL;
    p_trans_ = NULL;
    p_sock_ = NULL;
  }

  int32_t worker_func() {
    sock_lock_.Lock();
    if (!p_client_) {
      sock_lock_.Unlock();
      return -1;
    }

    try {
      // status = p_client_->process_request(request_item);
    } catch(...) {
      Clear();
      sock_lock_.Unlock();
      // WriteLog(kLogFatal, "process_request():%d failed", thread_num_);
      return -1;
    }

    sock_lock_.Unlock();

    return 0;
  }
  
 private:
  static void * TestActiveFunc(void *arg) {
    LongConnHandler *p_handler = reinterpret_cast<LongConnHandler*>(arg);
    assert(p_handler);

    while (true) {
      Sleep::DoSleep(p_handler->check_interval_ * 1000);

      /// 检查是否连通
      p_handler->sock_lock_.Lock();
      if (p_handler->p_client_) {
        try {
          int ret = p_handler->p_client_->test_active();
          if (ret < 0) {
            WriteLog(kLogFatal, "connection to %s:%d inactive", p_handler->peer_ip_.c_str(), p_handler->peer_port_);
          }
        } catch (...) {
          p_handler->Clear();
        }
      }

      /// 连接正常，则不重连
      if (p_handler->p_sock_) {
        p_handler->sock_lock_.Unlock();
        continue;
      }

      /// 重连
      bool ret = p_handler->Reconnect();
      p_handler->sock_lock_.Unlock();
      if (ret)
        WriteLog(kLogFatal, "Reconnect() to %s:%d OK", p_handler->peer_ip_.c_str(), p_handler->peer_port_);
      else
        WriteLog(kLogFatal, "Reconnect() to %s:%d failed", p_handler->peer_ip_.c_str(), p_handler->peer_port_);
    }
  }
  
  bool Reconnect() {
    try {
      p_sock_ = new TSocket(peer_ip_, peer_port_);
      p_sock_->setConnTimeout(socket_timeout_);
      p_sock_->setRecvTimeout(socket_timeout_);
      p_sock_->setSendTimeout(socket_timeout_);
      p_sock_->open();
      shared_ptr<TSocket> sock_ptr(p_sock_);

      p_trans_ = new TFramedTransport(sock_ptr);
      shared_ptr<TFramedTransport> trans_ptr(p_trans_);

      p_prot_ = new TBinaryProtocol(trans_ptr);
      shared_ptr<TBinaryProtocol> prot_ptr(p_prot_);

      p_client_ = new T(prot_ptr);
    } catch(...) {
      Clear();
      return false;
    }

    return true;
  }

 public:
  string peer_ip_;
  int peer_port_;
  int socket_timeout_;    ///< 网络超时
  int check_interval_;    ///< 链路检测的时间间隔，单位:秒

  TSocket *p_sock_;
  TFramedTransport *p_trans_;
  TBinaryProtocol *p_prot_;
  T *p_client_;

  Mutex sock_lock_;  ///< 保护p_sock_

  thread::Thread *p_thread_;
};
}}};  ///< end of namespace ganji::crawler::conf_crawler

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LONG_CONN_H_
