/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/thread.cc
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

#include "thread.h"
#include "sleep.h"

namespace ganji { namespace util { namespace thread {

int32_t Thread::thread_count_ = 0;

Thread::Thread() {
  mutex_resume_ = NULL;
  detached_ = false;
  started_ = false;
  semaphore_wait_.Create(0);
}

Thread::~Thread() {
  semaphore_wait_.Destroy();
  Reset();
}

Thread * Thread::CreateThread(__ganji_thread_proc *func, void *parm) {
  Thread * thread = new Thread;
  thread->mutex_resume_ = new Mutex;
  thread->func_ = func;
  thread->parm_ = parm;
  thread->mutex_resume_->Lock();
  if (pthread_create(&thread->thread_, NULL, GenericThreadProc, thread) != 0) {
    thread->mutex_resume_->Unlock();
    return NULL;
  }
  thread_count_++;
  return thread;
}

void Thread::FreeThread(Thread * thread) {
  if( thread && (!thread->detached_)) {
    delete thread;
  }
}

bool Thread::ResumeThread() {
  mutex_resume_->Unlock();
  return true;
}

bool Thread::WaitThread(void **stats) {
  while (!started_) {
    Sleep::DoSleep(100);
  }

  semaphore_wait_.Acquire();

  if (stats) *stats = return_;
  return true;
}

void * Thread::GenericThreadProc(void * arg) {
  Thread * thread = (Thread *) arg;
  int32_t detached = thread->detached_;

  pthread_detach(thread->thread_);

  thread->mutex_resume_->Lock();
  thread->started_ = true;

  thread->return_ = thread->func_(thread->parm_);

  thread->semaphore_wait_.Release();

  // a detached thread should delete itself, for there will
  // no other thread has this responsibility

  if (detached) delete thread;
  thread_count_--;
  return 0;
}

void Thread::Reset() {
  if (mutex_resume_) {
    delete mutex_resume_;
    mutex_resume_ = NULL;
  }
}

bool Thread::DetachThread() {
  detached_ = true;
  return true;
}
} } }  ///< end of namespace ganji::util::thread
