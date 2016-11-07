/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/dc/dc.cc
 * @namespace ganji::crawler::conf_crawler::dc
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/dc/dc.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <list>

#include "util/net/ip_num.h"
#include "util/net/http_opt.h"
#include "conf_crawler/dc/dc_config.h"
#include "conf_crawler/dc/conf_crawler_types.h"

#include "util/text/text.h"
#include "util/time/time.h"
#include "util/thread/sleep.h"
#include "util/encoding/gbk_utf8.h"
#include "util/log/thread_fast_log.h"

using std::list;
using std::string;
using std::vector;
using boost::shared_ptr;

namespace Http = ::ganji::util::net::Http;
namespace IpNum = ::ganji::util::net::IpNum;
namespace Text = ::ganji::util::text::Text;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace Time = ganji::util::time;
namespace GbkUtf8Conv = ganji::util::encoding::GbkUtf8Conv;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dc {
Dc::~Dc() {
  Thread::FreeThread(p_timer_thread_);
  Thread::FreeThread(p_proc_upload_task_thread_);
  Thread::FreeThread(p_upload_body_thread_);
}

int Dc::Init(DcConfig *p_config) {
  if (!p_config) {
    WriteLog(kLogFatal, "dc config NULL");
    return -1;
  }
  p_config_ = p_config;

  /// connection with link base
  link_base_conn_.Init(p_config_->GetLinkBaseHost(),
                       p_config_->GetLinkBasePort(),
                       p_config_->GetSocketTimeout(),
                       p_config_->GetPersistCount());


  /// create timer thread
  p_timer_thread_ = Thread::CreateThread(TimerThread, this);
  p_timer_thread_->ResumeThread();

  /// create process upload task thread
  p_proc_upload_task_thread_ = Thread::CreateThread(ProcUploadTaskThread, this);
  p_proc_upload_task_thread_->ResumeThread();

  /// create upload body thread
  p_upload_body_thread_ = Thread::CreateThread(UploadBodyThread, this);
  p_upload_body_thread_->ResumeThread();

  return 0;
}

/// pop from download_queue_, and push into download_map_
int Dc::OnGetTask(vector<DownloadTask> *p_list) {
  int _max_task = 50;
  list<DownloadTaskItem> task_list;
  download_lock_.Lock();
  while (!download_queue_.empty()) {
    task_list.push_back(download_queue_.front());
    download_queue_.pop();
    if (task_list.size() >= _max_task)
    {
      break;
    }
  }
  download_lock_.Unlock();

  if (task_list.empty()) {
    return -1;
  }

  cache_lock_.Lock();
  for (list<DownloadTaskItem>::iterator it = task_list.begin();
      it != task_list.end(); ++it) {
    DownloadTaskItem &task_item = *it;
    task_item.count_++;
    task_item.start_time_ = time(NULL);

    const string &url = task_item.req_item_.url;
    WriteLog(kLogDebug, "GetTask[%s]", url.c_str());

    DownloadTaskMap::iterator it = download_map_.find(url);
    if (it == download_map_.end()) {
      download_map_.insert(make_pair(url, task_item));
    } else {
      it->second.count_ = task_item.count_;
      it->second.start_time_ = task_item.start_time_;
    }

    DownloadTask download_task;
    download_task.req_item = task_item.req_item_;
    download_task.prop_item = task_item.prop_item_;
    p_list->push_back(download_task);
  }
  cache_lock_.Unlock();

  return 0;
}

/// push item into download_queue_
void Dc::PushTaskItem(const DownloadTaskItem &task_item) {
  download_lock_.Lock();
  download_queue_.push(task_item);
  download_lock_.Unlock();
}

/// push item into upload_task_queue_
int Dc::OnUploadTask(const DownloadedBodyItem &downloaded_body_item) {
  upload_task_lock_.Lock();
  upload_task_queue_.push(downloaded_body_item);
  upload_task_lock_.Unlock();
  upload_task_cond_.Signal();

  return 0;
}

void * Dc::TimerThread(void *arg) {
  Dc *p_dc = reinterpret_cast<Dc *>(arg);
  assert(p_dc);
  int check_interval = p_dc->p_config_->GetCheckInterval();
  time_t last_show_time = time(NULL);

  while (true) {
    Sleep::DoSleep(check_interval * 1000);
    
    p_dc->DelayTaskPush();

    p_dc->ExpireRequest();

    time_t now = time(NULL);
    int diff = now - last_show_time;
    if (diff >= 60) {
      last_show_time = now;

      p_dc->download_lock_.Lock();
      int task_queue_count = p_dc->download_queue_.size();
      p_dc->download_lock_.Unlock();

      p_dc->delay_download_lock_.Lock();
      int task_delay_queue_count = p_dc->delay_download_queue_.size();
      p_dc->delay_download_lock_.Unlock();

      p_dc->cache_lock_.Lock();
      int download_map_count = p_dc->download_map_.size();
      p_dc->cache_lock_.Unlock();

      p_dc->upload_task_lock_.Lock();
      int upload_task_count = p_dc->upload_task_queue_.size();
      p_dc->upload_task_lock_.Unlock();

      p_dc->downloaded_lock_.Lock();
      int downloaded_count = p_dc->downloaded_queue_.size();
      p_dc->downloaded_lock_.Unlock();

      WriteLog(kLogNotice, "#task queue:%d #delay task queue:%d #download map:%d #upload task:%d #downloaded:%d",
               task_queue_count,
               task_delay_queue_count,
               download_map_count,
               upload_task_count,
               downloaded_count);
    }
  }

  return NULL;
}

/// reenque/remove expired requests from download_map_ into download_queue_
void Dc::ExpireRequest() {
  int cache_expire_interval = p_config_->GetCacheExpireInterval();
  time_t cur_time_s = time(NULL);
  /// items needed to requeue
  list<DownloadTaskItem> reque_list;

  /// scan expired items in download_map_
  cache_lock_.Lock();
  for (DownloadTaskMap::iterator it = download_map_.begin();
      it != download_map_.end(); ) {
    const DownloadTaskItem &task_item = it->second;
    const string &url = task_item.req_item_.url;
    bool is_img = task_item.prop_item_.is_img;
    time_t start_time = task_item.start_time_;
    if (cur_time_s < start_time) {
      //WriteLog(kLogFatal, "cur_time_s[%lu] < start_time[%lu]", cur_time_s, start_time);
      ++it;
      continue;
    }

    if (cur_time_s-start_time >= cache_expire_interval) {
      string start_str;
      Time::GetY4MDHMS2(start_time, &start_str);
      int count = task_item.count_;
      bool is_drop = false;
      if (count > task_item.prop_item_.retry_times) {
        is_drop = true;
      } else {
        WriteLog(kLogWarning, "Reenque[%s] times[%d]", url.c_str(), count);
        reque_list.push_back(task_item);
      }
      int event = kLogWarning;
      if (is_img)
        event = kLogDebug;
      WriteLog(event, "Expire[%s] start time:%s drop:%d",
               url.c_str(),
               start_str.c_str(), is_drop);
      download_map_.erase(it++);
    } else {
      ++it;
    }
  }
  cache_lock_.Unlock();

  /// reenque into download_queue_
  download_lock_.Lock();
  for (list<DownloadTaskItem>::iterator it = reque_list.begin();
      it != reque_list.end(); ++it) {
    download_queue_.push(*it);
  }
  download_lock_.Unlock();
}

void * Dc::ProcUploadTaskThread(void *arg) {
  Dc *p_dc = reinterpret_cast<Dc *>(arg);

  while (true) {
    p_dc->ProcUploadTaskThreadFunc();
  }

  return NULL;
}

/// pop from upload_task_queue_ and push into downloaded_queue_
int Dc::ProcUploadTaskThreadFunc() {
  upload_task_lock_.Lock();
  while (upload_task_queue_.empty()) {
    upload_task_lock_.Unlock();
    upload_task_cond_.Wait();
    upload_task_lock_.Lock();
  }
  DownloadedBodyItem downloaded_body_item = upload_task_queue_.front();
  upload_task_queue_.pop();
  upload_task_lock_.Unlock();

  /// find url in download cache
  const string &url = downloaded_body_item.req_item.url;
  bool is_ok = downloaded_body_item.is_ok;
  bool is_found = true;
  DownloadTaskItem task_item;
  /// whether take from download_map_ into download_queue_
  bool en_queue = false;
  cache_lock_.Lock();
  do {
    DownloadTaskMap::iterator it = download_map_.find(url);
    if (it != download_map_.end()) {
      task_item = it->second;
      bool is_img = task_item.prop_item_.is_img;
      int retry_times = p_config_->GetRetryTimes();
      /// download fail, retry until exceed limit
      if (!is_ok) {
        if (task_item.count_ < retry_times || task_item.prop_item_.depth == 0) {
          en_queue = true;
        } else {
          WriteLog(kLogWarning, "ExceedLimit url[%s] down count[%d]", url.c_str(), task_item.count_);
        }
      }
      /// taken from download_map_ if is not img, just to expire img
      if (!is_img)
        download_map_.erase(it);
    } else {
      is_found = false;
    }
  } while (0);
  cache_lock_.Unlock();

  /// XXX upload to link base also if not found
  if (!is_found) {
    WriteLog(kLogWarning, "ProcUploadTaskThreadFunc, no[%s] in map", url.c_str());
    // return -1;
  }

  if (is_ok) {
    downloaded_lock_.Lock();
    downloaded_queue_.push(downloaded_body_item);
    downloaded_lock_.Unlock();
    downloaded_cond_.Signal();
    WriteLog(kLogDebug, "download[%s] OK", url.c_str());
  } else if (en_queue) {
    task_item.start_time_ += p_config_->GetFailedDelayTime(); 
    delay_download_lock_.Lock();
    delay_download_queue_.push(task_item);
    delay_download_lock_.Unlock();
    WriteLog(kLogNotice, "Reenqueue[%s] count[%u]", url.c_str(), task_item.count_);
  }

  return 0;
}

void Dc::DelayTaskPush()
{
  time_t Now = time(NULL); 
  list<DownloadTaskItem> task_list;
  DownloadTaskItem item;
  delay_download_lock_.Lock();
  while (!delay_download_queue_.empty()) {
    item = delay_download_queue_.front();
    if (item.start_time_ > Now)
    {
      break;
    }

    task_list.push_back(item);
    delay_download_queue_.pop();
  }
  delay_download_lock_.Unlock();

  if (task_list.empty()) {
    return ;
  }

  
  for (list<DownloadTaskItem>::iterator it = task_list.begin();it != task_list.end(); ++it) {
    DownloadTaskItem &task_item = *it;
    PushTaskItem(task_item);
  }
  
}


void * Dc::UploadBodyThread(void *arg) {
  Dc *p_dc = reinterpret_cast<Dc *>(arg);

  while (true) {
    p_dc->UploadBodyThreadFunc();
  }

  return NULL;
}

int Dc::UploadBodyThreadFunc() {
  downloaded_lock_.Lock();
  while (downloaded_queue_.empty()) {
    downloaded_lock_.Unlock();
    downloaded_cond_.Wait();
    downloaded_lock_.Lock();
  }
  DownloadedBodyItem downloaded_body_item = downloaded_queue_.front();
  downloaded_queue_.pop();
  downloaded_lock_.Unlock();

  const string &url = downloaded_body_item.req_item.url;
  const string &body = downloaded_body_item.body;
  /// convert from gbk to utf8
  if (GbkUtf8Conv::IsGbkStr(body)) {
    string body_utf8;
    int is_ok = GbkUtf8Conv::GbkToUtf8(body, &body_utf8);
    if (is_ok == 0)
      downloaded_body_item.body = body_utf8;
  }

  /// upload to link base
  uint32_t start_t = Time::GetCurTimeMs();
  int ret = UploadBody(downloaded_body_item);
  uint32_t end_t = Time::GetCurTimeMs();
  int diff = end_t - start_t;
  if (ret < 0) {
    WriteLog(kLogNotice, "UploadBody()[%s] failed, elapsed:%dms", url.c_str(), diff);
    return ret;
  } else {
    WriteLog(kLogDebug, "UploadBody()[%s] OK, elapsed:%dms", url.c_str(), diff);
  }

  return 0;
}

int Dc::UploadBody(const DownloadedBodyItem &downloaded_body_item) {
  int ret = 0;
  link_base_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (link_base_conn_.NeedReset()) {
      bool is_ok = link_base_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetRetryTimes(); i++) {
      try {
        link_base_conn_.Client()->upload_download_task(downloaded_body_item);
        link_base_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(...) {
        ret = -1;
        bool is_ok = link_base_conn_.Reset();
        if (!is_ok) {
          WriteLog(kLogNotice, "upload_download_task() failed, times:%d Reset failed", i);
          break;
        } else {
          WriteLog(kLogNotice, "upload_download_task() failed, times:%d Reset OK", i);
        }
      }
    }
  } while (0);
  link_base_conn_.Unlock();

  return ret;
}

DcManager::~DcManager() {
  if (p_get_task_thread_) {
    int get_task_thread_count = p_config_->GetTaskThreadCount();
    for (int i = 0; i < get_task_thread_count; i++)
      Thread::FreeThread(p_get_task_thread_[i]);
    delete []p_get_task_thread_;
  }
}

int DcManager::Init(DcConfig *p_config) {
  if (!p_config) {
    WriteLog(kLogFatal, "dc manager config NULL");
    return -1;
  }
  p_config_ = p_config;

  /// Init GbkUtf8Conv
  const string &gbk_utf16_file = p_config_->GetGbkUtf16File();
  if (GbkUtf8Conv::Init(gbk_utf16_file) < 0) {
    WriteLog(kLogFatal, "GbkUtf8Conv Init failed");
    return -1;
  }

  /// connection with link base
  link_base_conn_.Init(p_config_->GetLinkBaseHost(),
                       p_config_->GetLinkBasePort(),
                       p_config_->GetSocketTimeout(),
                       p_config_->GetPersistCount());

  /// create get task thread
  int get_task_thread_count = p_config_->GetTaskThreadCount();
  p_get_task_thread_ = new ThreadPtr[get_task_thread_count];
  for (int i = 0; i < get_task_thread_count; i++) {
    p_get_task_thread_[i] = Thread::CreateThread(GetTaskThread, this);
    p_get_task_thread_[i]->ResumeThread();
  }

  /// normal dc Init
  if (normal_dc_.Init(p_config) < 0) {
    return -1;
  }
  /// webkit dc Init
  if (webkit_dc_.Init(p_config) < 0) {
    return -1;
  }

  return 0;
}

/// based on download_type
int DcManager::OnGetTask(vector<DownloadTask> *p_list, const DownloaderType::type downloader_type) {
  if (downloader_type == DownloaderType::type::NORMAL_TYPE) {
    return normal_dc_.OnGetTask(p_list);
  } else if (downloader_type == DownloaderType::type::WEBKIT_TYPE) {
    return webkit_dc_.OnGetTask(p_list);
  } else {
    WriteLog(kLogFatal, "OnGetTask() invalid downloader_type:%d", downloader_type);
    return -1;
  }
}

/// push task into download_queue_ based on download_type
void DcManager::OnPushTask(const DownloadTask &download_task) {
  DownloaderType::type downloader_type = download_task.req_item.downloader_type;
  if (downloader_type != DownloaderType::type::NORMAL_TYPE &&
      downloader_type != DownloaderType::type::WEBKIT_TYPE) {
    WriteLog(kLogFatal, "OnPushTask() invalid downloader_type:%d", downloader_type);
    return;
  }

  DownloadTaskItem task_item;
  task_item.req_item_ = download_task.req_item;
  task_item.prop_item_ = download_task.prop_item;

  /// fill in task item
  if (FillTaskItem(&task_item) < 0)
    return;

  if (downloader_type == DownloaderType::type::NORMAL_TYPE) {
    return normal_dc_.PushTaskItem(task_item);
  } else {
    return webkit_dc_.PushTaskItem(task_item);
  }
}

/// based on download_type
int DcManager::OnUploadTask(const DownloadedBodyItem &downloaded_body_item) {
  DownloaderType::type downloader_type = downloaded_body_item.req_item.downloader_type;
  if (downloader_type == DownloaderType::type::NORMAL_TYPE) {
    return normal_dc_.OnUploadTask(downloaded_body_item);
  } else if (downloader_type == DownloaderType::type::WEBKIT_TYPE) {
    return webkit_dc_.OnUploadTask(downloaded_body_item);
  } else {
    WriteLog(kLogFatal, "OnUploadTask() invalid downloader_type:%d", downloader_type);
    return -1;
  }
}

void * DcManager::GetTaskThread(void *arg) {
  DcManager *p_dc_manager = reinterpret_cast<DcManager *>(arg);
  assert(p_dc_manager);
  int time_slice = p_dc_manager->p_config_->GetTimeSlice();

  while (true) {
    Sleep::DoSleep(time_slice);

    p_dc_manager->GetTaskThreadFunc();
  }

  return NULL;
}

/// get item from link base, and push item into responding dc based on download type 
int DcManager::GetTaskThreadFunc() {
  int ret = 0;
  vector<DownloadTask> task_list;
  link_base_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (link_base_conn_.NeedReset()) {
      bool is_ok = link_base_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetRetryTimes(); i++) {
      try {
        link_base_conn_.Client()->get_download_task(task_list);
        link_base_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(...) {
        ret = -1;
        bool is_ok = link_base_conn_.Reset();
        if (!is_ok) {
          WriteLog(kLogNotice, "get_download_task() failed, times:%d Reset failed", i);
          break;
        } else {
          WriteLog(kLogNotice, "get_download_task() failed, times:%d Reset OK", i);
        }
      }
    }
  } while (0);
  link_base_conn_.Unlock();

  /// push into corresponding dc
  for (vector<DownloadTask>::iterator it = task_list.begin();
      it != task_list.end(); ++it) {
    const DownloadTask &download_task = *it;
    OnPushTask(download_task);
  }

  return ret;
}

int DcManager::FillTaskItem(DownloadTaskItem *p_task_item) {
  DownloadTaskItem &task_item = *p_task_item;

  /// fill intime out
  int &time_out = task_item.req_item_.time_out;
  int default_timeout = p_config_->GetDownloadTimeout();
  if (time_out < default_timeout)
    time_out = default_timeout;

  /// fill in user agent
  p_config_->GetUserAgent(&task_item.req_item_.ua);

  /// dns lookup to fill in ip
  const string &url = task_item.req_item_.url;
  if (GetUrlIp(url, &task_item.req_item_.ip) < 0)
    return -1;

  return 0;
}

int DcManager::GetUrlIp(const string &url, string *p_ip) {
  string domain;
  int ret = Http::GetUrlDomain(url, &domain);
  if (ret < 0) {
    WriteLog(kLogFatal, "GetUrlDomain[%s] failed", url.c_str());
    return -1;
  }

  /// lookup dns to get ip
  string ip;
  domain_ip_lock_.RdLock();
  DnsCacheMap::iterator it_dns = dns_cache_map_.find(domain);
  if (it_dns != dns_cache_map_.end()) {
    ip = it_dns->second;
  }
  domain_ip_lock_.Unlock();

  if (ip.empty()) {
    ret = DnsLookup(domain, &ip);
    if (ret < 0) {
      return -1;
    } else {
      domain_ip_lock_.WrLock();
      dns_cache_map_[domain] = ip;
      domain_ip_lock_.Unlock();
    }
  }

  *p_ip = ip;
  return 0;
}

int DcManager::DnsLookup(const string &domain, string *p_ip) {
  char buf[1024];
  struct hostent host;
  struct hostent *p_host;
  int ret = gethostbyname_r(domain.c_str(),
      &host,
      buf,
      sizeof(buf),
      &p_host,
      &h_errno);

  if (ret != 0) {
    WriteLog(kLogFatal, "gethostbyname_r[%s] failed:%d", domain.c_str(), h_errno);
    return -1;
  }
  uint32_t ip_num = static_cast<uint32_t>((reinterpret_cast<struct in_addr*>(host.h_addr))->s_addr);
  ret = IpNum::Num2Ip(ip_num, p_ip);
  if (ret < 0) {
    WriteLog(kLogFatal, "Num2Ip failed:%s", strerror(errno));
    return -1;
  }
  return 0;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dc

