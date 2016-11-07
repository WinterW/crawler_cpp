/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file          ganji/util/thread/mutex.cc
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "mutex.h"
#include "mutex_imp.h"

namespace ganji { namespace util { namespace thread {

Mutex::Mutex() {
  mutex_imp_ = new MutexImp();
}

Mutex::~Mutex() {
  if (mutex_imp_) {
    delete mutex_imp_;
  }
  mutex_imp_ = 0;
}

bool Mutex::Lock() {
  return mutex_imp_->Lock();
}

bool Mutex::Unlock() {
  return mutex_imp_->Unlock();
}

MutexGuard::MutexGuard(Mutex *lock) : lock_(lock), is_lock_owner_(false) {
  lock_->Lock();
  is_lock_owner_ = true;
}

MutexGuard::~MutexGuard() {
  if (is_lock_owner_) {
    lock_->Unlock();
  }
}
} } }  ///< end of namespace ganji::util::thread
