/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/thread/thread_heartbeat.cc
* @namespace ganji::util::thread
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/

#include "thread_heartbeat.h"
#include "util/log/thread_fast_log.h"
#include "util/thread/atomic_ops.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>

using namespace ganji::util::log::ThreadFastLog;
using ganji::util::thread::ThreadHeartbeat;

#define CHECK_INFO_NUM   100

ThreadHeartbeat* ThreadHeartbeat::inst_ = NULL;

ThreadHeartbeat* ThreadHeartbeat::GetInstance() {
  if (NULL == inst_) {
    inst_ = new (std::nothrow) ThreadHeartbeat();
    if (NULL == inst_){
      WriteLog(kLogFatal, "new ThreadHeartbeat err.");
      return false;
    }
  }
  return inst_;
}

bool ThreadHeartbeat::Init(const time_t heartbeat_time) {
  check_info_ = new (std::nothrow) HeartbeatInfo[CHECK_INFO_NUM];
  if (check_info_ == NULL) {
    WriteLog(kLogFatal, "create check_info_ failed.");
    return false;
  }
  for (int i = 0; i < CHECK_INFO_NUM; ++i) {
    check_info_[i].last_time_ = 0;
  }
  heartbeat_time_ = heartbeat_time;
  monitoring_thread_num_ = 0; 

  pthread_t monitoring_thr;
  ///< 启动监控线程
  if (0 != pthread_create(&monitoring_thr, NULL, ThreadHeartbeat::CheckThreadAlive, NULL)) {
    WriteLog(kLogFatal, "Failed to start CheckThreadAlive.\n");
    CloseLog(0);
    return false;
  }
  WriteLog(kLogNotice, "CheckThreadAlive thread launched successfully.");
  WriteLog(kLogNotice, "Init succeed!  heartbeat_time_ = %u", heartbeat_time_);
  return true;
}

void ThreadHeartbeat::HeartbeatUpdate(const int handle) {
  time_t now = time(NULL);
  if (handle >= CHECK_INFO_NUM) {
    WriteLog(kLogWarning, "handle err = %d", handle);
    return;
  }
  check_info_[handle].last_time_ = now;
}

void ThreadHeartbeat::HeartbeatUpdate(const int handle, const time_t now) {
  if (handle >= CHECK_INFO_NUM) {
    WriteLog(kLogWarning, "handle err = %d", handle);
    return;
  }
  check_info_[handle].last_time_ = now;
}

int ThreadHeartbeat::GetHandle(const string &thread_name) {
  if (monitoring_thread_num_ >= CHECK_INFO_NUM) {
    return -1;
  }
  int handle = atomic_add(&monitoring_thread_num_, 1);
  check_info_[handle-1].thread_name_ = thread_name;
  check_info_[handle-1].tid_ = pthread_self();
  return handle-1;
}

void *ThreadHeartbeat::CheckThreadAlive(void *arg) {
  ThreadHeartbeat* th = ThreadHeartbeat::GetInstance();  
  while (true) {
    sleep(th->heartbeat_time_);
    time_t now = time(NULL);
    for (int i = 0; i <= th->monitoring_thread_num_; ++i) {
      int interval = now - th->check_info_[i].last_time_;
      if ((interval > th->heartbeat_time_) && (th->check_info_[i].last_time_ != 0)) { 
        WriteLog(kLogWarning, "thread_name: %s, tid = %d, now = %u, last_time = %u", 
                              th->check_info_[i].thread_name_.c_str(), th->check_info_[i].tid_, 
                              now, th->check_info_[i].last_time_);
      }
    }
    WriteLog(kLogNotice, "thread heartbeat monitor alive");
  }
  return NULL;
}
