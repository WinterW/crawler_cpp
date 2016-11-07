/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file          ganji/util/thread/mutex.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an obj-language implement of mutex
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 *
 */

#ifndef _GANJI_UTIL_THREAD_MUTEX_H_
#define _GANJI_UTIL_THREAD_MUTEX_H_

#include "global.h"

namespace ganji { namespace util { namespace thread {

/// 前置声明， mutex functions 的具体实现类
class MutexImp;

/**
 * @class: ganji::util::thread::Mutex
 * @brief: "Mutual Exclusive" 对象
 *          这是一个thread safe的类
 * 
 */
class Mutex {
 public:
  Mutex();
  virtual ~Mutex();

  /// If the Mutex is currently unlocked, it becomes locked and owned by the calling thread and returns immediately.
  /// If the Mmtex is already locked by another thread, it will suspends the calling thread until the Unlock is called.
  /// 注意， 如果是同一线程内多次Lock，可能会有不同的表现， 一般情况下是都可以获取到锁并立刻返回，但必须全部释放，别的线程才可以再次获得锁
  /// @return true:success, false:error
  bool Lock();

  /// Unlock Mutex
  /// @return true:success, false:error
  bool Unlock();

 protected:
  MutexImp *mutex_imp_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

/**
  * @class: ganji::util::thread::MutexGuard
  * @breaf: 一个Mutex的哨兵类, 常用在以下情况：
  *         一个函数在获得锁后， 必须return才能释放锁。 使用哨兵可以避免在某个return语句前忘记Unlock
  */
class MutexGuard {
 public:
  explicit MutexGuard(Mutex *lock);
  ~MutexGuard();

 private:
  bool    is_lock_owner_;
  Mutex*  lock_;

 private:
  DISALLOW_COPY_AND_ASSIGN(MutexGuard);
};
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_MUTEX_H_
