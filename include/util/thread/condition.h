/**
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/condition.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an obj-language condition function.
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_CONDITION_H_
#define _GANJI_UTIL_THREAD_CONDITION_H_

#include "global.h"

namespace ganji { namespace util { namespace thread {

/// 前置声明, condition 功能的具体实现类
class ConditionImp;

/**
 * @class:  ganji::util::thread::Condition
 * @brief:  阻塞调用了Wait的线程，直到Signal或者broadcast被调用， 支持超时.
 *          这是一个thread safe的类
 */
class Condition {
 public:
  Condition();
  virtual ~Condition();

  /// 阻塞调用了Wait的线程，直到Signal或者broadcast被调用
  void Wait();

  /// 阻塞调用了Wait的线程，直到Signal或者broadcast被调用,或者timeout
  /// @return true:if restarted by Signal; false:if timeout
  bool Wait(uint32_t nMilliseconds);

  /// 唤醒一个阻塞在condition上的线程, 如果没有阻塞的线程， 则什么也不会发生
  /// 最多只能唤醒一个线程， 而且无法由调用者指定唤醒哪一个线程
  void Signal();

  /// 唤醒所有阻塞在condition上的线程,如果没有阻塞的线程， 则什么也不会发生
  void Broadcast();

 protected:
  ConditionImp * condition_imp_;  ///< implement class of the functions.

 private:
  DISALLOW_COPY_AND_ASSIGN(Condition);
};
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_CONDITION_H_
