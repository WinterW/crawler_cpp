/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/semaphore_imp.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * implement an obj-language semaphore by pthread_cond
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_SEMAPHORE_IMP_H_
#define _GANJI_UTIL_THREAD_SEMAPHORE_IMP_H_

#include <pthread.h>
#include "global.h"

namespace ganji { namespace util { namespace thread {

class SemaphoreImp {
 public:
  SemaphoreImp() {
  };
  virtual ~SemaphoreImp() {
  };

  bool Create(int32_t initial);
  bool Destroy();
  bool Acquire();
  bool TimeAcquire(int32_t milliseconds);
  bool Release();

 protected:
  pthread_mutex_t lock_;          ///< Serialize access to internal state.
  pthread_cond_t count_nonzero_;  ///< Block until there are no waiters.
  uint32_t count_;                ///< Count of the semaphore.
  uint32_t waiters_;              ///< Number of threads that have called <ACE_OS::sema_wait>.

 private:
  DISALLOW_COPY_AND_ASSIGN(SemaphoreImp);
};

inline bool SemaphoreImp::Create(int32_t initial) {
  int32_t result = -1;
  if ( (pthread_mutex_init(&lock_, NULL) == 0) &&
      (pthread_cond_init(&count_nonzero_, NULL) == 0) &&
      (pthread_mutex_lock(&lock_) == 0) ) {
    count_ = initial;
    waiters_ = 0;
    if (pthread_mutex_unlock(&lock_) == 0) {
      result = 0;
    }
  }

  if (result == -1) {
    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&count_nonzero_);
  }

  return (result == 0) ? true : false;
}

inline bool SemaphoreImp::Destroy() {
  int32_t r1 = pthread_mutex_destroy(&lock_);
  int32_t r2 = pthread_cond_destroy(&count_nonzero_);
  return ((r1 == 0) && (r2 == 0)) ? true : false;
}

inline bool SemaphoreImp::Acquire() {
  if (pthread_mutex_lock(&lock_) != 0) {
    return false;
  }

  // Keep track of the number of waiters so that we can signal
  // them properly in <ACE_OS::sema_post>.
  waiters_++;

  int32_t result = 0;

  // Wait until the semaphore count is > 0.
  while (count_ == 0) {
    if ((result = pthread_cond_wait(&count_nonzero_, &lock_)) != 0) {
      break;
    }
  }

  --waiters_;

  if (result == 0) {
    --count_;
  }
  pthread_mutex_unlock(&lock_);

  return (result == 0) ? true : false;
}

inline bool SemaphoreImp::TimeAcquire(int32_t milliseconds) {
  struct timespec to;

  if (milliseconds == 0) {
    milliseconds = 5;
  }

  to.tv_sec = time(NULL) + milliseconds / 1000;
  to.tv_nsec = (milliseconds % 1000) * 10000000;

  if (pthread_mutex_lock(&lock_) != 0) {
    return false;
  }

  // Keep track of the number of waiters so that we can signal them properly.
  waiters_++;

  int32_t result = 0;
  // Wait until the semaphore count is > 0 or until time out.
  while (count_ == 0) {
    if ((result = pthread_cond_timedwait(&count_nonzero_, &lock_, &to)) != 0) {
      break;
    }
  }

  --waiters_;

  if (result == 0) {
    --count_;
  }
  pthread_mutex_unlock(&lock_);

  return (result == 0);
}

inline bool SemaphoreImp::Release() {
  int32_t result = -1;
  if (pthread_mutex_lock(&lock_) == 0) {
    // Always allow a waiter to continue if there is one.
    if (waiters_ > 0) {
      result = pthread_cond_signal(&count_nonzero_);
    } else {
      result = 0;
    }
    count_++;
    pthread_mutex_unlock(&lock_);
  }
  return result == 0 ? true : false;
}
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_SEMAPHORE_IMP_H_
