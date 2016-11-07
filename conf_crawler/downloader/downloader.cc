/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/downloader.cc
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-26
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "downloader.h"

#include <iostream>
#include <list>

#include "downloader_config.h"
#include "conf_crawler/downloader/conf_crawler_types.h"
#include "curl_downloader.h"
#include "downloader_util.h"
#include "net_checker.h"

#include "util/text/text.h"
#include "util/time/time.h"
#include "util/thread/sleep.h"
#include "util/system/system.h"
#include "util/log/thread_fast_log.h"

using std::list;
using boost::shared_ptr;

namespace Text = ::ganji::util::text::Text;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace thread = ganji::util::thread;
namespace Time = ganji::util::time;
namespace System = ::ganji::util::system::System;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;
using thread::Thread;

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
Downloader::~Downloader() {
  thread::Thread::FreeThread(p_timer_thread_);
  thread::Thread::FreeThread(p_get_task_thread_);
  thread::Thread::FreeThread(p_upload_body_thread_);
  int download_thread_count = p_config_->GetDownloadThreadCount();
  if (p_download_thread_) {
    for (int i = 0; i < download_thread_count; i++)
      thread::Thread::FreeThread(p_download_thread_[i]);
    delete []p_download_thread_;
  }
  if (p_download_thread_arg_)
    delete []p_download_thread_arg_;
  if (p_downloader_)
    delete []p_downloader_;

  if (p_config_->IsNetCheck()) {
    if (p_net_checker_)
      delete p_net_checker_;
  }
}

int Downloader::Init(DownloaderConfig *p_config) {
  if (!p_config) {
    WriteLog(kLogFatal, "downloader config NULL");
    return -1;
  }

  p_config_ = p_config;

  if (p_config_->IsNetCheck()) {
    p_net_checker_ = new NetChecker();
    if (p_net_checker_->Init(p_config_) < 0) {
      WriteLog(kLogFatal, "NetChecker Init failed");
      return -1;
    }
  }

  /// connect with dc
  dc_conn_.Init(p_config_->GetDcHost(),
                p_config_->GetDcPort(),
                p_config_->GetSocketTimeout(),
                p_config_->GetPersistCount());

  /// create timer thread
  p_timer_thread_ = thread::Thread::CreateThread(TimerThread, this);
  p_timer_thread_->ResumeThread();

  /// create get task thread
  p_get_task_thread_ = thread::Thread::CreateThread(GetTaskThread, this);
  p_get_task_thread_->ResumeThread();

  /// create upload body thread
  p_upload_body_thread_ = thread::Thread::CreateThread(UploadBodyThread, this);
  p_upload_body_thread_->ResumeThread();

  /// create curl downloader thread
  int download_thread_count = p_config_->GetDownloadThreadCount();
  p_downloader_ = new CurlDownloader[download_thread_count];
  p_download_thread_ = new ThreadPtr[download_thread_count];
  p_download_thread_arg_ = new ThreadArg[download_thread_count];
  for (int i = 0; i < download_thread_count; i++) {
    if (p_downloader_[i].Init(p_config_) < 0)
      return -1;
    p_download_thread_arg_[i] = ThreadArg(this, i);
    p_download_thread_[i] = thread::Thread::CreateThread(DownloadThread, &p_download_thread_arg_[i]);
    p_download_thread_[i]->ResumeThread();
  }

  return 0;
}

void Downloader::Run() {
  while (true) {
    Sleep::DoSleep(1000);
  }
}

void * Downloader::TimerThread(void *arg) {
  Downloader *p_downloader = reinterpret_cast<Downloader *>(arg);
  assert(p_downloader);
  int time_slice = p_downloader->p_config_->GetTimeSlice();
  time_t last_show_time = time(NULL);

  while (true) {
    Sleep::DoSleep(time_slice);
    p_downloader->TimerThreadFunc();

    time_t now = time(NULL);

    /// show queue size
    if (now-last_show_time > 60) {
      p_downloader->download_lock_.Lock();
      int download_queue_size = p_downloader->download_queue_.size();
      p_downloader->download_lock_.Unlock();

      p_downloader->friendly_lock_.Lock();
      int friendly_queue_size = p_downloader->friendly_queue_.size();
      p_downloader->friendly_lock_.Unlock();

      p_downloader->upload_lock_.Lock();
      int upload_count = p_downloader->upload_queue_.size();
      p_downloader->upload_lock_.Unlock();


      int mb = 0;
      System::GetMem(&mb);
      WriteLog(kLogNotice, "#queue:%d #friend queue:%d #upload:%d mem:%dMB",
               download_queue_size,
               friendly_queue_size,
               upload_count,
               mb);
      last_show_time = now;
    }
  }

  return NULL;
}

int Downloader::TimerThreadFunc() {
  //time_t now = time(NULL);
  uint64_t now = 0;
  Time::GetCurTimeMs(&now);
  friendly_lock_.Lock();
  list<FriendlyTask> task_list;
  while (!friendly_queue_.empty()) {
    FriendlyTask friendly_task = friendly_queue_.top();
    if (friendly_task.hint_time_ > now)
      break;
    task_list.push_back(friendly_task);
    friendly_queue_.pop();
  }
  friendly_lock_.Unlock();

  download_lock_.Lock();
  for (list<FriendlyTask>::iterator it = task_list.begin();
      it != task_list.end(); ++it) {
    download_queue_.push_front(it->task_);
  }
  download_lock_.Unlock();
  download_cond_.Signal();

  return 0;
}

void * Downloader::GetTaskThread(void *arg) {
  Downloader *p_downloader = reinterpret_cast<Downloader *>(arg);
  assert(p_downloader);
  int time_slice = p_downloader->p_config_->GetTimeSlice();

  while (true) {
    Sleep::DoSleep(time_slice);

    p_downloader->GetTaskThreadFunc();
  }

  return NULL;
}

int Downloader::GetTaskThreadFunc() {
  /// outer network is unreachable
  if (p_config_->IsNetCheck() && !p_net_checker_->IsNetOk()) {
    return -1;
  }

  int ret = 0;
  vector<DownloadTask> task_list;
  dc_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (dc_conn_.NeedReset()) {
      bool is_ok = dc_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetUploadRetryTimes(); i++) {
      try {
        dc_conn_.Client()->get_download_task(task_list, DownloaderType::type::NORMAL_TYPE);
        dc_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(...) {
        ret = -1;
        bool is_ok = dc_conn_.Reset();
        if (!is_ok) {
          WriteLog(kLogNotice, "get_download_task() failed, times:%d Reset failed", i);
          break;
        } else {
          WriteLog(kLogNotice, "get_download_task() failed, times:%d Reset OK", i);
        }
      }
    }
  } while (0);
  dc_conn_.Unlock();

  if (task_list.empty())
    return 0;

  for (vector<DownloadTask>::iterator it_task = task_list.begin();
      it_task != task_list.end(); ++it_task) {
    const DownloadTask &download_task = *it_task;
    const string &url = download_task.req_item.url;
    /// download friendly
    bool is_friendly = download_task.prop_item.is_friendly;
    bool enqueue = true;
    //modified by wangsj
    int32_t interval = download_task.prop_item.interval;
    //end
    if (is_friendly && interval > 0) {
      //delete by wangsj
      //int32_t interval = download_task.prop_item.interval;
      uint32_t uinterval = (uint)interval; 
      string domain;
      string fKey;
      ret = DownloaderUtil::GetMainDomain(url, &domain);
      if (ret == 0) {
        time_t last_time = 0;
        fKey = domain + download_task.req_item.ip;// Use domain + dst ip as the key
        friendly_lock_.Lock();
        Domain2TimeMap::iterator it_domain = domain_time_map_.find(fKey);
        if (it_domain != domain_time_map_.end()) {
          last_time = it_domain->second;
        }
        //modified by wangsj
        //time_t now = time(NULL);
        //time_t next_time = last_time + interval;
        uint64_t now = 0;
        Time::GetCurTimeMs(&now);
        uint64_t next_time = last_time + uinterval;
        //end
        if (now < next_time) {
          domain_time_map_[fKey] = next_time;
          FriendlyTask task_cache(download_task, next_time);
          friendly_queue_.push(task_cache);
          enqueue = false;
          WriteLog(kLogNotice, "GetTask[%s], friendly, down time[%lu]", url.c_str(), next_time);
        } else {
          domain_time_map_[fKey] = now;
          WriteLog(kLogNotice, "GetTask[%s], friendly, down immediately", url.c_str());
        }
        friendly_lock_.Unlock();
      }
    } else {
      WriteLog(kLogNotice, "GetTask[%s], unfriendly", url.c_str());
    }

    if (enqueue) {
      download_lock_.Lock();
      download_queue_.push_back(download_task);
      download_lock_.Unlock();
      download_cond_.Signal();
    }
  }

  return 0;
}

void * Downloader::UploadBodyThread(void *arg) {
  Downloader *p_downloader = reinterpret_cast<Downloader *>(arg);

  while (true) {
    p_downloader->UploadBodyThreadFunc();
  }

  return NULL;
}

int Downloader::UploadBodyThreadFunc() {
  upload_lock_.Lock();
  while (upload_queue_.empty()) {
    upload_lock_.Unlock();
    upload_cond_.Wait();
    upload_lock_.Lock();
  }
  DownloadedBodyItem upload_body_item = upload_queue_.front();
  upload_queue_.pop();
  upload_lock_.Unlock();

  const string &url = upload_body_item.req_item.url;
  /// upload body to dc
  uint32_t start_t = Time::GetCurTimeMs();
  int ret = UploadBody(upload_body_item);
  uint32_t end_t = Time::GetCurTimeMs();
  int diff = end_t - start_t;
  if (ret < 0) {
    WriteLog(kLogNotice, "UploadBody() [%s] failed, elapsed:%dms", url.c_str(), diff);
    return ret;
  } else {
    WriteLog(kLogNotice, "UploadBody() [%s] OK, elapsed:%dms", url.c_str(), diff);
  }

  return 0;
}

int Downloader::UploadBody(const DownloadedBodyItem &downloaded_body_item) {
  int ret = 0;
  dc_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (dc_conn_.NeedReset()) {
      bool is_ok = dc_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetUploadRetryTimes(); i++) {
      try {
        dc_conn_.Client()->upload_download_task(downloaded_body_item);
        dc_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(...) {
        ret = -1;
        bool is_ok = dc_conn_.Reset();
        if (!is_ok) {
          WriteLog(kLogNotice, "upload_download_task() failed, times:%d Reset failed", i);
          break;
        } else {
          WriteLog(kLogNotice, "upload_download_task() failed, times:%d Reset OK", i);
        }
      }
    }
  } while (0);
  dc_conn_.Unlock();

  return ret;
}

void * Downloader::DownloadThread(void *arg) {
  ThreadArg *p_arg = reinterpret_cast<ThreadArg *>(arg);
  Downloader *p_downloader = p_arg->p_downloader;
  int idx = p_arg->idx;

  while (true) {
    p_downloader->DownloadThreadFunc(idx);
  }

  return NULL;
}

int Downloader::DownloadThreadFunc(int idx) {
  download_lock_.Lock();
  while (download_queue_.empty()) {
    download_lock_.Unlock();
    download_cond_.Wait();
    download_lock_.Lock();
  }
  DownloadTask download_task = download_queue_.front();
  download_queue_.pop_front();
  download_lock_.Unlock();

  const DownloadReqItem &req_item = download_task.req_item;
  int depth = download_task.prop_item.depth;
  HttpReqItem http_item(req_item.url,
      req_item.ip,
      req_item.referer,
      req_item.ua,
      req_item.post_fields,
      req_item.time_out,
      req_item.header_fields_type,
      depth);
  DownloadedBodyItem body_item;
  int ret = p_downloader_[idx].Perform(http_item, &body_item.body);

  body_item.req_item = download_task.req_item;
  body_item.prop_item = download_task.prop_item;
  if (ret < 0) {
    body_item.is_ok = false;
  } else {
    body_item.is_ok = true;
  }

  upload_lock_.Lock();
  upload_queue_.push(body_item);
  upload_lock_.Unlock();
  upload_cond_.Signal();

  return ret;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

