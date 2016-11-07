/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/rwlock.cc
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

#include "rwlock.h"
#include "rwlock_imp.h"

namespace ganji { namespace util { namespace thread {

RWLock::RWLock() {
  rwlock_imp_ = new RWLockImp;
}

RWLock::~RWLock() {
  delete rwlock_imp_;
}

bool RWLock::RdLock() {
  return rwlock_imp_->Rdlock();
}

bool RWLock::WrLock() {
  return rwlock_imp_->Wrlock();
}

bool RWLock::Unlock() {
  return rwlock_imp_->Unlock();
}

RWLockGuard::RWLockGuard(RWLockGuard::GuardType type, RWLock *lock) : lock_(lock), is_owner_(false) {
  if (type == RWLockGuard::READTYPE) {
    lock_->RdLock();
  } else if (type == RWLockGuard::WRITETYPE) {
    lock_->WrLock();
  } else {
    lock_->RdLock();
  }
  is_owner_ = true;
}

RWLockGuard::~RWLockGuard() {
  if (is_owner_) {
    lock_->Unlock();
  }
}
} } }  ///< end of namespace ganji::util::thread
