/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/semaphore.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an obj-language implement of semaphore
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_SEMAPHORE_H_
#define _GANJI_UTIL_THREAD_SEMAPHORE_H_

#include "global.h"

namespace ganji { namespace util { namespace thread {

/// 前置声明， semaphore功能的具体实现类
class SemaphoreImp;

/**
 * @class: ganji::util::thread::Semaphore
 * @brief: Semaphore 对象.
 *          这是一个thread safe的类
 */
class Semaphore {
 public:
  Semaphore();
  virtual ~Semaphore();

  /// 创建一个Semaphore, 允许被acquired nInitialCount 次.
  /// @param[in] nInitialCount : max times that acquired at same time.
  /// @return true:success, false:failed.
  bool Create(int32_t nInitialCount);

  /// 销毁Semaphore， 无论是否已经被acquire了。
  /// @return true:success, false:failed.
  bool Destroy();

  /// 如果没有达到nInitialCount次，则acquire一次， 否则阻塞线程直到Semaphore被Release
  /// @return true:success, false:failed.
  bool Acquire();

  /// 如果没有达到nInitialCount次，则acquire一次， 否则阻塞线程直到超时，或者有Release
  /// @return true:success, false:failed or timeout
  bool Acquire(uint32_t nMilliseconds);

  /// 释放一个Semaphore.
  /// @return true:success, false:failed.
  bool Release();

 protected:
  SemaphoreImp * semaphore_imp_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Semaphore);
};
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_SEMAPHORE_H_
