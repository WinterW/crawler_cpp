/**
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/condition.cc
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

#include "condition.h"
#include "condition_imp.h"

namespace ganji { namespace util { namespace thread {

Condition::Condition() {
  condition_imp_ = new ConditionImp;
}

Condition::~Condition() {
  delete condition_imp_;
}

void Condition::Wait() {
  condition_imp_->Wait();
}

bool Condition::Wait(uint32_t nMilliseconds) {
  return condition_imp_->TimedWait(nMilliseconds);
}

void Condition::Signal() {
  condition_imp_->Signal();
}

void Condition::Broadcast() {
  condition_imp_->Broadcast();
}
} } }  ///< end of namespace ganji::util::thread
