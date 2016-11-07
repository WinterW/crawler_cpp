/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/rwlock.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an obj-language implement of rwlock
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_RWLOCK_H_
#define _GANJI_UTIL_THREAD_RWLOCK_H_

#include "global.h"

namespace ganji { namespace util { namespace thread {

/// 前置声明， 读写锁具体实现类
class RWLockImp;

/**
 * @class: ganji::util::thread::RWLock
 * @brief: Provide "Read-Write Lock" objects.
 *          当读锁时， 如果后续继续要求获取读锁， 都可以获得。  如果后续要求获得写锁，则该写锁被阻塞，直到该写锁前所有的读锁释放。
 *          当写锁时， 则后续无论写锁或者读锁的请求， 都被阻塞， 直到该写锁释放
 *          这是一个thread safe的类
 */
class RWLock {
 public:
  RWLock();
  virtual ~RWLock();

  /// 获取读锁
  /// @return true:success, false:failed
  bool RdLock();

  /// 获取写锁
  /// @return true:success, false:failed
  bool WrLock();

  /// 释放锁， 无论写锁还是读锁， 都由该函数释放
  /// @return true:success, false:failed
  bool Unlock();

 protected:
  RWLockImp * rwlock_imp_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RWLock);
};

class RWLockGuard {
 public:
  enum GuardType {READTYPE = 0, WRITETYPE = 1};

 public:
  RWLockGuard(RWLockGuard::GuardType type, RWLock *lock);
  ~RWLockGuard();

 private:
  RWLock * lock_;
  bool is_owner_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RWLockGuard);
};
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_RWLOCK_H_
