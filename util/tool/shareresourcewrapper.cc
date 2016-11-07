/**
 * @Copyright 2010 Ganji Inc.
 * @file    src/ganji/util/tool/shareresourcewrapper.cc
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  jiafazhang
 * @date    2010-08-26
 *
 * Define a class wrap resource that shared by many threads
 *
 * Change Log:
 *
 */

#include "shareresourcewrapper.h"
#include "util/thread/sleep.h"

namespace ganji { namespace util { namespace tool {
bool ShareResourceWrapper::Init(int number) {
  if (number > 0) {
    is_init_ = true;
    res_num_ = number;
    available_num = res_num_;
    for (int i = 0; i < res_num_; ++i) {
      res_state_.push_back(true);
    }
    return true;
  }
  return false;
}

int ShareResourceWrapper::GetResource(int wait_time) {
  while (true) {
    //ganji::util::thread::MutexGuard guard(&res_mutex_);
    res_mutex_.Lock();
    if (is_init_) {
      //res_mutex_.Lock();
      int avai_Id = -1;
      int retry_count = 0;
      while ((avai_Id = GetAvailableResource()) == -1 && retry_count < 5) {
        // pthread_cond_wait(&res_cond_, &res_mutex_);
        ++retry_count;
      }
      if (avai_Id != -1) {
        res_state_[avai_Id] = false;
        --available_num;
        //res_mutex_.Unlock();
        // pthread_cond_signal(&res_cond_);
        res_mutex_.Unlock();
        return avai_Id;
      }
    }
    res_mutex_.Unlock();
    ganji::util::thread::Sleep::DoSleep(5);
  }
  return -1;
}

bool ShareResourceWrapper::ReleaseResource(int resoure_Id) {
  ganji::util::thread::MutexGuard guard(&res_mutex_);
  if (resoure_Id >= 0 && resoure_Id < res_num_) {
    //res_mutex_.Lock();
    res_state_[resoure_Id] = true;
    ++available_num;
    //res_mutex_.Unlock();
  }
  return true;
}
} } }
