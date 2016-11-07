/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/thread.h
 * @namespace     ganji::util::thread
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an obj-language implement of thread
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_THREAD_H_
#define _GANJI_UTIL_THREAD_THREAD_H_

#include <pthread.h>
#include "global.h"
#include "mutex.h"
#include "semaphore.h"

namespace ganji { namespace util { namespace thread {

typedef void *__ganji_thread_proc(void *arg);

/**
 * @class: ganji::util::thread::Thread
 * @PURPOSE:  实现一个线程对象
 */
class Thread {
 public:

  /// 创建一个线程实例， 当创建后， 该线程不会立刻运行，直到 ResumeThread() 函数被调用
  /// @param[in] func main function of thread.
  /// @param[in] parm parameter of thread man funcion.
  /// @return Return NULL if failed to create instance of Thread obj, else return the point of the thread object
  static Thread * CreateThread(__ganji_thread_proc *func, void *parm);
  
  /// 回收那些没有 Detach 的线程, 同时释放线程所占的资源
  /// @param[in] thread   object that will free
  static void FreeThread(Thread * thread);

  /// 立刻运行线程
  /// @return true If succee, else return false.
  bool ResumeThread();

  /// 阻塞，直到线程运行结束
  ///     注意， 必须是没有Detach的线程，调用才管用,否则一直阻塞
  /// @param[out] stats The return status.
  /// @return true If succee, else return false.
  bool WaitThread(void **stats);

  /// 调用此函数Detach线程， 这样， 不用操心线程结束后的资源释放， 线程结束后， 会自动释放线程所有资源
  /// 注意， 只释放线程占用的所有资源， 但是不会释放Thread obj。 所以， 对于那些频繁创建和销毁的线程，不应该使用此方式
  /// @return true If succee, else return false.
  bool DetachThread();

 public:
  static int32_t thread_count_;
  pthread_t thread_;

 protected:
  Thread();
  virtual ~Thread();

 private:
  /// generic function that call main function of the thread.
  static void * GenericThreadProc(void * arg);

  /// reset the status of thread.
  void Reset();

 private:
  __ganji_thread_proc *func_;
  void *parm_;                ///< point to the parament of main function of the thread.
  void *return_;              ///< return value of the thread
  bool detached_;             ///< DetachThread or not
  Mutex *mutex_resume_;
  Semaphore semaphore_wait_;
  bool started_;              ///< whether the thread runned or not
};
} } }   ///< end of namespace ganji::util::thread
#endif  ///< _GANJI_UTIL_THREAD_THREAD_H_
