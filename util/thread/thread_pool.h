/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/thread_pool.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an implement of threadpool
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_THREAD_POOL_H_
#define _GANJI_UTIL_THREAD_THREAD_POOL_H_


#include <vector>
#include <queue>

#include "ganji/ganji_global.h"
#include "ganji/util/thread/thread.h"
#include "ganji/util/thread/sleep.h"

namespace Sleep = ganji::util::thread::Sleep;

namespace ganji { namespace util { namespace thread {

class ThreadPoolFunc {
 public:
  ThreadPoolFunc() { }
  virtual ~ThreadPoolFunc() { }
  virtual bool DoFunc(void *arg) = 0;
  virtual void AfterFunc(void *arg, bool is_exec_succ) = 0;

 public:
  char *point_threadpool_;
};

template<class QNode>
class ThreadPool {
 public:
  explicit ThreadPool(uint32_t threadnum);
  ~ThreadPool();

  static void * __Thread_Func(void * para);
  void RegisterFunc(ThreadPoolFunc * func);
  bool Start();
  void Stop();

  bool SetRequest(QNode * reqnode);

  std::queue<QNode> * GetFailQueue() {
    return &queue_failed_reqs_;
  }

  void ClearFailQueue();

 private:
  bool stop_flag_;
  uint32_t num_threads_;
  std::vector<Thread *> pool_threads_;
  Semaphore semaphore_wait_;

  std::queue<QNode> queue_reqs_;
  Mutex queue_reqs_mutex_;
  std::queue<QNode> queue_failed_reqs_;
  Mutex queue_failed_reqs_mutex_;

  ThreadPoolFunc *thread_func_;
  bool is_register_thread_func_;
};

#define __ThreadPool_Func template<class QNode>

__ThreadPool_Func void * ThreadPool<QNode>::__Thread_Func(void * para) {
  ThreadPool<QNode> * pThis = (ThreadPool<QNode> *)para;

  QNode node;
  while (!pThis->stop_flag_) {
    pThis->semaphore_wait_.Acquire();
    if (pThis->stop_flag_) {
      break;
    }
    do {
      pThis->queue_reqs_mutex_.Lock();
      if (pThis->queue_reqs_.empty()) {
        pThis->queue_reqs_mutex_.Unlock();
        break;
      }
      node = pThis->queue_reqs_.front();
      pThis->queue_reqs_.pop();
      pThis->queue_reqs_mutex_.Unlock();

      bool is_exec_succ = true;
      if (!pThis->thread_func_->DoFunc(&node)) {
        is_exec_succ = false;
        pThis->queue_failed_reqs_mutex_.Lock();
        if (pThis->queue_failed_reqs_.size() < 100) {
          pThis->queue_failed_reqs_.push(node);
        }
        pThis->queue_failed_reqs_mutex_.Unlock();
      }
      pThis->thread_func_->AfterFunc(&node, is_exec_succ);
    }while (true);
  }
  return 0;
}

__ThreadPool_Func ThreadPool<QNode>::ThreadPool(uint32_t threadnum):num_threads_(threadnum) {
  is_register_thread_func_ = false;
  stop_flag_ = true;
  semaphore_wait_.Create(threadnum);
}

__ThreadPool_Func ThreadPool<QNode>::~ThreadPool() {
  Stop();
}

__ThreadPool_Func void ThreadPool<QNode>::RegisterFunc(ThreadPoolFunc * func) {
  is_register_thread_func_ = true;
  thread_func_ = func;
  func->point_threadpool_ = reinterpret_cast<char *>(this);
}

  __ThreadPool_Func bool ThreadPool<QNode>::Start() {
    if ( !stop_flag_ )
      return false;
    if (!is_register_thread_func_)
      return false;
    if ( 0 == num_threads_ )
      return false;
    for (uint32_t i = 0; i < num_threads_; i++) {
      Thread * thread = Thread::CreateThread(ThreadPool<QNode>::__Thread_Func, this);
      if ( !thread ) {
        while ( !pool_threads_.empty() ) {
          Thread::FreeThread(*(pool_threads_.begin()));
          pool_threads_.erase(pool_threads_.begin());
        }
        return false;
      }
      pool_threads_.push_back(thread);
    }

    stop_flag_ = false;
    for (uint32_t i = 0; i < num_threads_; i++) {
      Thread * thread = pool_threads_[i];
      thread->ResumeThread();
    }
    return true;
  }

  __ThreadPool_Func void ThreadPool<QNode>::Stop() {
    if (stop_flag_)
      return;

    stop_flag_ = true;
    for (int i = 0; i < num_threads_; i++) {
      semaphore_wait_.Release();
    }
    Sleep::DoSleep(100);

    for (uint32_t i = 0; i < num_threads_; i++) {
      Thread * thread = pool_threads_[i];
      thread->WaitThread(0);
      Thread::FreeThread(thread);
    }
    pool_threads_.resize(0);
  }

  __ThreadPool_Func bool ThreadPool<QNode>::SetRequest(QNode *node) {
    if (stop_flag_)
      return false;

    queue_reqs_mutex_.Lock();
    queue_reqs_.push(*node);
    queue_reqs_mutex_.Unlock();

    semaphore_wait_.Release();

    return true;
  }

__ThreadPool_Func void ThreadPool<QNode>::ClearFailQueue() {
  queue_failed_reqs_mutex_.Lock();
  while (!queue_failed_reqs_.empty()) {
    queue_failed_reqs_.pop();
  }
  queue_failed_reqs_mutex_.Unlock();
}
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_THREAD_POOL_H_
