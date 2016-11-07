/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/semaphore.cc
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

#include "semaphore.h"
#include "semaphore_imp.h"

namespace ganji { namespace util { namespace thread {

Semaphore::Semaphore() {
  semaphore_imp_ = new SemaphoreImp;
}

Semaphore::~Semaphore() {
  delete semaphore_imp_;
}

bool Semaphore::Create(int32_t nInitialCount) {
  return semaphore_imp_->Create(nInitialCount);
}

bool Semaphore::Destroy() {
  return semaphore_imp_->Destroy();
}

bool Semaphore::Acquire() {
  return semaphore_imp_->Acquire();
}

bool Semaphore::Acquire(uint32_t nMilliseconds) {
  return semaphore_imp_->TimeAcquire(nMilliseconds);
}

bool Semaphore::Release() {
  return semaphore_imp_->Release();
}
} } }  ///< end of namespace ganji::util::thread
