/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file          ganji/util/thread/condition_imp.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * implement an thread-safe obj-language condition by pthread_cond
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_CONDITION_IMP_H_
#define _GANJI_UTIL_THREAD_CONDITION_IMP_H_

#include <pthread.h>
#include "global.h"

namespace ganji { namespace util { namespace thread {

/**
 * @class: ganji::util::thread::ConditionImp
 * @brief: 实现了线程安全的condition功能
*/
class ConditionImp {
 public:
  ConditionImp() {
    pthread_cond_init(&condition_, NULL);
    pthread_mutex_init(&mutex_, NULL);
    is_signaled_ = 0;
  };
  ~ConditionImp() {
    pthread_cond_destroy(&condition_);
    pthread_mutex_destroy(&mutex_);
  };

  bool Wait();
  bool TimedWait(int32_t milliseconds);
  bool Signal();
  bool Broadcast();
  bool Reset();

 protected:
  pthread_cond_t condition_;
  pthread_mutex_t mutex_;
  int32_t is_signaled_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ConditionImp);
};

inline bool ConditionImp::Wait() {
  if (pthread_mutex_lock(&mutex_) != 0) {
    return false;
  }

  int err = 0;
  while (is_signaled_ == 0 && err == 0) {
    err = pthread_cond_wait(&condition_, &mutex_);
  }
  if (err == 0)
    is_signaled_ = 0;
  /*
  if (is_signaled_) {
    is_signaled_ = 0;
    pthread_mutex_unlock(&mutex_);
    return true;
  }
  int err = pthread_cond_wait(&condition_, &mutex_);
  */
  pthread_mutex_unlock(&mutex_);

  return (err == 0);
}

inline bool ConditionImp::TimedWait(int32_t milliseconds) {
  struct timespec to;
  to.tv_sec = time(NULL) + milliseconds / 1000;
  to.tv_nsec = (milliseconds % 1000) * 10000000;

  if (pthread_mutex_lock(&mutex_) != 0) {
    return false;
  }

  if (is_signaled_) {
    is_signaled_ = 0;
    pthread_mutex_unlock(&mutex_);
    return true;
  }

  int32_t err = pthread_cond_timedwait(&condition_, &mutex_, &to);
  pthread_mutex_unlock(&mutex_);

  return (err == 0);
}

inline bool ConditionImp::Signal() {
  if (pthread_mutex_lock(&mutex_) != 0) {
    return false;
  }

  if (is_signaled_) {
    pthread_mutex_unlock(&mutex_);
    return true;
  }

  int err = pthread_cond_signal(&condition_);
  is_signaled_ = 1;

  pthread_mutex_unlock(&mutex_);

  return true;
}

inline bool ConditionImp::Broadcast() {
  int err = pthread_cond_broadcast(&condition_);
  return (err == 0);
}

inline bool ConditionImp::Reset() {
  return false;
}
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_CONDITION_IMP_H_
