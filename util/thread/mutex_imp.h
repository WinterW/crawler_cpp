/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/thread/mutex_imp.h
 * @namespace ganji::util::thread
 * @version 1.0
 * @author  haohuang
 * @date    2010-07-20
 * 
 * implement an obj-language mutex by pthread_mutex
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_MUTEX_IMP_H_
#define _GANJI_UTIL_THREAD_MUTEX_IMP_H_

#include <pthread.h>
#include "global.h"

namespace ganji { namespace util { namespace thread {

class MutexImp {
 public:
  MutexImp() {
    pthread_mutex_init(&pthread_mutex_, NULL);
  }
  ~MutexImp() {
    pthread_mutex_destroy(&pthread_mutex_);
  };

  bool Lock();
  bool Unlock();

 private:
  pthread_mutex_t pthread_mutex_;

 private:
  DISALLOW_COPY_AND_ASSIGN(MutexImp);
};


inline bool MutexImp::Lock() {
  pthread_mutex_lock(&pthread_mutex_);
  return true;
}

inline bool MutexImp::Unlock() {
  pthread_mutex_unlock(&pthread_mutex_);
  return true;
}
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_MUTEX_IMP_H_
