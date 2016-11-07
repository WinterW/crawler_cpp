/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/thread/rwlock_guard.h
 * @namespace ganji::util::thread
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-21
 * 
 */
#ifndef _GANJI_UTIL_THREAD_RWLOCK_GUARD_H_
#define _GANJI_UTIL_THREAD_RWLOCK_GUARD_H_

#include <pthread.h>

namespace ganji { namespace util { namespace thread {
class RWLockRead {
 public:
  explicit RWLockRead(pthread_rwlock_t* rwlock) : rwlock_(rwlock) {
    pthread_rwlock_rdlock(rwlock_);
  }

  ~RWLockRead() {
    pthread_rwlock_unlock(rwlock_);
  }

 protected:
  pthread_rwlock_t* rwlock_;
};

class RWLockWrite {
 public:
  explicit RWLockWrite(pthread_rwlock_t* rwlock) : rwlock_(rwlock) {
    pthread_rwlock_wrlock(rwlock_);
  }

  ~RWLockWrite() {
    pthread_rwlock_unlock(rwlock_);
  }

 protected:
  pthread_rwlock_t* rwlock_;
};
}}}
#endif  ///< _GANJI_UTIL_THREAD_RWLOCK_GUARD_H_
