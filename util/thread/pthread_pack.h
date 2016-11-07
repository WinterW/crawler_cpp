/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/thread/pthread_pack.h
 * @namespace ganji::util::thread
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-21
 * 
 */

#ifndef _GANJI_UTIL_THREAD_PTHREAD_PACK_H_
#define _GANJI_UTIL_THREAD_PTHREAD_PACK_H_

#include <pthread.h>
#include "ganji/util/log/thread_fast_log.h"
using namespace ganji::util::log::ThreadFastLog;

namespace ganji { namespace util { namespace thread {

inline void Pthread_mutex_init(pthread_mutex_t* mutex, __const pthread_mutexattr_t* mutexattr) {
  if (pthread_mutex_init(mutex, mutexattr) != 0) {
    WriteLog(kLogFatal, "pthread_mutex_init() failed");
    abort();
  }
}

inline void Pthread_mutex_destroy(pthread_mutex_t* mutex) {
  if (pthread_mutex_destroy(mutex) != 0) {
    WriteLog(kLogWarning, "Pthread_mutex_destory() failed");
  }
}

inline void Pthread_mutex_lock(pthread_mutex_t* mutex) {
  if (pthread_mutex_lock(mutex) != 0) {
    WriteLog(kLogFatal, "pthread_mutex_lock(%p) failed", mutex);
    abort();
  }
}

inline void Pthread_mutex_unlock(pthread_mutex_t* mutex) {
  if (pthread_mutex_unlock(mutex) != 0) {
    WriteLog(kLogFatal, "pthread_mutex_unlock(%p) failed", mutex);
    abort();
  }
}

inline void Pthread_cond_init(pthread_cond_t* __restrict cond,
    __const pthread_condattr_t* __restrict cond_attr) {
  if (pthread_cond_init(cond, cond_attr) != 0) {
    WriteLog(kLogFatal, "pthread_cond_init() failed");
    abort();
  }
}

inline void Pthread_cond_destroy(pthread_cond_t* cond) {
  if (pthread_cond_destroy(cond) != 0) {
    WriteLog(kLogWarning, "pthread_cond_destroy() failed");
  }
}

inline int Pthread_cond_signal(pthread_cond_t* cond) {
  int ret = pthread_cond_signal(cond);
  if ( ret != 0) {
    WriteLog(kLogWarning, "pthread_cond_signal(%p) failed", cond);
  }
  return ret;
}

inline int Pthread_cond_wait(pthread_cond_t *__restrict cond,
    pthread_mutex_t *__restrict mutex) {
  int ret = pthread_cond_wait(cond, mutex);
  if ( ret != 0) {
    WriteLog(kLogWarning, "pthread_cond_wait(%p) failed", cond);
  }
  return ret;
}
}}}

#endif  ///< _GANJI_UTIL_THREAD_PTHREAD_PACK_H_
