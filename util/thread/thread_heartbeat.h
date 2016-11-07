/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/thread/thread_heartbeat.h
* @namespace ganji::util::thread
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
* @effect  监控应用程序线程是否存活
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/

#ifndef _GANJI_UTIL_THREAD_THREAD_HEARTBEAT_H
#define _GANJI_UTIL_THREAD_THREAD_HEARTBEAT_H

#include <string>
#include <pthread.h>
using std::string;

namespace ganji { namespace util { namespace thread {

typedef volatile int atomic_t;

struct HeartbeatInfo {
  string thread_name_;
  pthread_t tid_;
  time_t last_time_;
};

///< 具体使用方法可参考cpc_searcher中的cpc_core_data.cc cpc_core_data.h文件
class ThreadHeartbeat {
 public:

  /// 初始化,应用程序在分线程之前调用此函数进行初始化
  /// @param[in] heartbeat_time: 心跳检测周期
  /// @return true:success, false:failed.
  bool Init(const time_t heartbeat_time);

  /// 想被检测的线程调用此函数更新信息,应用程序在监控的线程中调用此函数
  /// @param[in] thread_name: 线程名字
  void HeartbeatUpdate(const int handle);

  /// 想被检测的线程调用此函数更新信息,应用程序在监控的线程中调用此函数
  /// @param[in] now: time(NULL)获取到的当前时间
  void HeartbeatUpdate(const int handle, const time_t now);

  /// 返回线程所在句柄,被检测的线程调用此函数调用此函数
  /// @param[in] thread_id: 查找的线程ID
  /// @return 句柄位置
  int GetHandle(const string &thread_name);

  /// 检测线程
  static void *CheckThreadAlive(void *arg);

  static ThreadHeartbeat* inst_;
  static ThreadHeartbeat* GetInstance();
 private:
  HeartbeatInfo *check_info_;
  int heartbeat_time_;
  atomic_t monitoring_thread_num_;
};
} } }
#endif ///<  _GANJI_UTIL_THREAD_THREAD_HEARTBEAT_H
