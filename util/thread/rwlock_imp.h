/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/rwlock_imp.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * implement an obj-language rwlock base pthread_rwlock
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_RWLOCK_IMP_H_
#define _GANJI_UTIL_THREAD_RWLOCK_IMP_H_

#include <pthread.h>
#include "global.h"

namespace ganji { namespace util { namespace thread {

class RWLockImp {
 public:
  RWLockImp() {
    pthread_rwlock_init(&rwlock_, NULL);
  };
  ~RWLockImp() {
    pthread_rwlock_destroy(&rwlock_);
  };

  bool Rdlock();
  bool Wrlock();
  bool Unlock();

 protected:
  pthread_rwlock_t rwlock_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RWLockImp);
};

bool RWLockImp::Rdlock() {
  pthread_rwlock_rdlock(&rwlock_);
  return true;
}

bool RWLockImp::Wrlock() {
  pthread_rwlock_wrlock(&rwlock_);
  return true;
}

bool RWLockImp::Unlock() {
  pthread_rwlock_unlock(&rwlock_);
  return true;
}
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_RWLOCK_IMP_H_
