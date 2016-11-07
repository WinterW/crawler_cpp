/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/thread/blocking_queue.h
 * @namespace ganji::util::thread
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-21
 * 
 */

#ifndef _GANJI_UTIL_THREAD_BLOCKING_QUEUE_H_
#define _GANJI_UTIL_THREAD_BLOCKING_QUEUE_H_

#include <queue>
#include <deque>

namespace ganji { namespace util { namespace thread {

class LockGuard {
 public:
  explicit LockGuard(pthread_mutex_t* lock) : lock_(lock) {
    pthread_mutex_lock(lock_);
  }

  ~LockGuard() {
    pthread_mutex_unlock(lock_);
  }

 private:
  pthread_mutex_t* lock_;
};

template <class ItemType, class QueueType = std::deque<ItemType> >
class BlockingQueue {
 public:
  BlockingQueue() {
    pthread_mutex_init(&queue_mutex_, NULL);
    pthread_cond_init(&queue_cond_, NULL);
  }

  ~BlockingQueue() {
    pthread_mutex_destroy(&queue_mutex_);
    pthread_cond_destroy(&queue_cond_);
  }

  void Add(const ItemType& Object) {
    pthread_mutex_lock(&queue_mutex_);
    queue_.push(Object);
    pthread_mutex_unlock(&queue_mutex_);
    pthread_cond_signal(&queue_cond_);
  }

  ItemType Take() {
    ItemType ret;
    pthread_mutex_lock(&queue_mutex_);
    while (queue_.size() == 0) {
      pthread_cond_wait(&queue_cond_, &queue_mutex_);
    }
    ret = queue_.front();
    queue_.pop();
    pthread_mutex_unlock(&queue_mutex_);
    return ret;
  }

  int Take(ItemType& value) {
    pthread_mutex_lock(&queue_mutex_);
    while (queue_.size() == 0) {
      pthread_cond_wait(&queue_cond_, &queue_mutex_);
    }
    value = queue_.front();
    queue_.pop();
    pthread_mutex_unlock(&queue_mutex_);
    return 0;
  }

  size_t Size(bool real = false) {
    if (real) {
      LockGuard guard(&queue_mutex_);
      return queue_.size();
    }
    return queue_.size();
  }

 private:
  std::queue<ItemType, QueueType> queue_;
  pthread_cond_t queue_cond_;
  pthread_mutex_t queue_mutex_;
};
}}}

#endif  ///< _GANJI_UTIL_THREAD_BLOCKING_QUEUE_H_
