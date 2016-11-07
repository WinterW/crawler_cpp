/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/md5_dedup.cc
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-23
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/url_dedup/md5_dedup.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ganji/util/encoding/md5_generator.h"
#include "ganji/util/log/thread_fast_log.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/system/system.h"
#include "ganji/util/time/time.h"
#include "ganji/util/text/text.h"
#include "ganji/crawler/conf_crawler/url_dedup/dedup_config.h"

using std::string;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

using ganji::util::encoding::MD5Generator;
using ganji::util::thread::Thread;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace System = ::ganji::util::system::System;
namespace Time = ::ganji::util::time;
namespace Text = ::ganji::util::text::Text;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
Md5Dedup::~Md5Dedup() {
  if (p_set_list_) {
    for (int i = 0; i < day_count_; i++)
      if (p_set_list_[i] != NULL)
        delete p_set_list_[i];
    delete []p_set_list_;
  }

  Thread::FreeThread(p_timer_thread_);
}

int Md5Dedup::Init(DedupConfig *p_config) {
  p_config_ = p_config;

  day_count_ = p_config_->DayCount();
  if (day_count_ <= 0) {
    WriteLog(kLogFatal, "#day[%d] invalid", day_count_);
    return -1;
  }

  /// allocate memory for set list
  p_set_list_ = new UrlMd5SetPtr[day_count_];
  memset(p_set_list_, 0, sizeof(p_set_list_) * day_count_);
  /// allocate set for today only
  CreateTodaySet();

  /// load md5 from saved path
  LoadSavedMd5();

  /// create directory for md5
  const string &md5_path = p_config_->Md5Path(); 
  int ret = mkdir(md5_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
  if (0 == ret) {
    WriteLog(kLogNotice, "mkdir:%s OK", md5_path.c_str());
  } else if (errno != EEXIST) {
    WriteLog(kLogNotice, "mkdir:%s failed:%s", md5_path.c_str(), strerror(errno));
    return -1;
  }

  /// start timer thread
  string time_str;
  Time::GetY4MDHMS2(time(NULL), &time_str);
  hour_ = time_str.substr(8, 2);

  Time::GetY4MD2(time(NULL), &today_);
  p_timer_thread_ = Thread::CreateThread(TimerThread, this);
  p_timer_thread_->ResumeThread();

  return 0;
}

void Md5Dedup::IsExists(const string &url, DedupExistItem *p_exist_item) {
  UrlMd5 url_md5;
  if (GetUrlMd5(url, &url_md5) < 0) {
    p_exist_item->is_exists = false;
    return;
  }

  bool is_exists = false;
  string item_info;
  url_lock_.RdLock();
  for (int i = 0; i < day_count_; i++) {
    const UrlMd5Set *p_set = p_set_list_[i];
    if (!p_set)
      continue;
    UrlMd5Set::const_iterator it = p_set->find(url_md5);
    if (it != p_set->end()) {
      is_exists = true;
      const UrlMd5 url_md5 = *it;
      time_t day_time = time(NULL) - i * 3600 * 24;
      string date;
      Time::GetY4MD2(day_time, &date);
      p_exist_item->item_info = "date:" + date;
      break;
    }
  }
  url_lock_.Unlock();

  p_exist_item->is_exists = is_exists;
}

bool Md5Dedup::Insert(const string &url) {
  UrlMd5 url_md5;
  if (GetUrlMd5(url, &url_md5) < 0) {
    return false;
  }

  bool ret = true;
  url_lock_.WrLock();
  UrlMd5Set *p_today = p_set_list_[0];
  if (p_today) {
    p_today->insert(url_md5);
  } else {
    ret = false;
    WriteLog(kLogFatal, "today set NULL");
    CreateTodaySet();
  }
  url_lock_.Unlock();

  return ret;
}

bool Md5Dedup::TestExistsAndInsert(const string &url, bool *p_exists) {
  UrlMd5 url_md5;
  if (GetUrlMd5(url, &url_md5) < 0) {
    return false;
  }

  *p_exists = false;
  url_lock_.WrLock();
  for (int i = 0; i < day_count_; i++) {
    UrlMd5Set *p_set = p_set_list_[i];
    if (!p_set)
      continue;

    UrlMd5Set::iterator it = p_set->find(url_md5);
    if (it != p_set->end()) {
      *p_exists = true;
      break;
    }
  }
  if (!*p_exists) {
    p_set_list_[0]->insert(url_md5);
  }
  url_lock_.Unlock();

  return true;
}

bool Md5Dedup::Remove(const std::string &url) {
  UrlMd5 url_md5;
  if (GetUrlMd5(url, &url_md5) < 0) {
    return false;
  }

  bool found = false;
  url_lock_.WrLock();
  UrlMd5Set *p_today = p_set_list_[0];
  if (p_today) {
    UrlMd5Set::iterator it = p_today->find(url_md5);
    if (it != p_today->end()) {
      found = true;
      p_today->erase(it);
    }
  } else {
    found = false;
    WriteLog(kLogFatal, "today set NULL");
    CreateTodaySet();
  }
  url_lock_.Unlock();

  return found;
}

void Md5Dedup::Info(string *p_info) {
  p_info->clear();
  (*p_info) += "#day:" + Text::IntToStr(day_count_) + "\n";

  url_lock_.RdLock();
  for (int i = 0; i < day_count_; i++) {
    time_t day_time = time(NULL) - i * 3600 * 24;
    string date;
    Time::GetY4MD2(day_time, &date);

    int item_count = 0;
    if (p_set_list_[i])
      item_count = p_set_list_[i]->size();
    (*p_info) += "date:" + date + "\t#item:" + Text::IntToStr(item_count) + "\n";
  }
  url_lock_.Unlock();
}

/// allocate set for today only
void Md5Dedup::CreateTodaySet() {
  p_set_list_[0] = new UrlMd5Set;
  int bucket_count = p_config_->BucketCount();
  p_set_list_[0]->rehash(bucket_count);
}

void Md5Dedup::LoadSavedMd5() {
  MD5Generator md5_generator;

  const string &md5_path = p_config_->Md5Path(); 
  int bucket_count = p_config_->BucketCount();
  for (int i = 0; i < day_count_; i++) {
    time_t ts = time(NULL) - i * 3600 * 24;
    string date;
    Time::GetY4MD2(ts, &date);
    string md5_file = md5_path + "/" + date;

    WriteLog(kLogNotice, "---------------------");
    WriteLog(kLogNotice, "start to load[%s]", md5_file.c_str());
    /// XXX reference
    UrlMd5Set *&p_set = p_set_list_[i];
    /// create set
    if (!p_set) {
      p_set = new UrlMd5Set;

      p_set->rehash(bucket_count);
      int mb = 0;
      System::GetMem(&mb);
      WriteLog(kLogNotice, "after rehash mem:%dMB", mb);
    }

    FILE *fp = fopen(md5_file.c_str(), "r");
    if (!fp) {
      WriteLog(kLogFatal, "open:%s failed:%s", md5_file.c_str(), strerror(errno));
      continue;
    }

    size_t n = 0;
    char *p_line = NULL;
    while (getline(&p_line, &n, fp) != -1) {
      if (strlen(p_line) <= 1)
        continue;
      p_line[strlen(p_line)-1] = '\0';
      string line = p_line;
      size_t pos = line.find('\t');
      if (pos == string::npos)
        continue;
      string md5_str = line.substr(0, pos);
      string time_str = line.substr(pos+1);
      UrlMd5 url_md5;
      if (time_str.size() == 6) {
        url_md5.hour = atoi(time_str.substr(0, 2).c_str());
        url_md5.minute = atoi(time_str.substr(2, 2).c_str());
        url_md5.sec = atoi(time_str.substr(4, 2).c_str());
      }

      md5_generator.FromString(md5_str);

      memcpy(url_md5.m, md5_generator.Digest(), MD5_DIGEST_LENGTH);

      p_set->insert(url_md5);
    }
    /// rehash to save memory
    int cur_size = p_set->size();
    p_set->rehash(2*cur_size);

    if (p_line)
      free(p_line);
    fclose(fp);

    int mb = 0;
    System::GetMem(&mb);
    WriteLog(kLogNotice, "#md5:%lu mem:%dMB", p_set->size(), mb);
  }
}

int Md5Dedup::GetUrlMd5(const string &url, UrlMd5 *p_url_md5) {
  MD5Generator md5_generator;
  if (md5_generator.Generate(url) < 0) {
    WriteLog(kLogFatal, "Generate Md5[%s] failed", url.c_str());
    return -1;
  }
  unsigned char *dig = md5_generator.Digest();
  memcpy(p_url_md5->m, dig, MD5_DIGEST_LENGTH);

  time_t now = time(NULL);
  struct tm tm_now;
  localtime_r(&now, &tm_now);
  p_url_md5->hour = tm_now.tm_hour;
  p_url_md5->minute = tm_now.tm_min;
  p_url_md5->sec = tm_now.tm_sec;

  return 0;
}

int Md5Dedup::SwitchDay() {
  url_lock_.WrLock();
  /// delete the set for oldest day
  UrlMd5Set *p_oldest = p_set_list_[day_count_-1];
  if (p_oldest)
    delete p_oldest;

  /// rehash today set to save memory
  if (p_set_list_[0]) {
    int cur_size = p_set_list_[0]->size();
    p_set_list_[0]->rehash(2*cur_size);
  }

  /// mv each day into the day before it
  for (int i = day_count_-1; i > 0; i--) {
    p_set_list_[i] = p_set_list_[i-1];
  }

  /// allocate a new day
  CreateTodaySet();

  url_lock_.Unlock();

  url_lock_.RdLock();
  int ret = SaveYesterdayMd5();
  url_lock_.Unlock();

  return ret;
}

int Md5Dedup::SwitchHour() {
  time_t now = time(NULL);
  string today;
  Time::GetY4MD2(now, &today);
  string md5_file = p_config_->Md5Path() + "/" + today;

  url_lock_.RdLock();
  UrlMd5Set *p_today = p_set_list_[0];
  int ret = SaveMd5File(p_today, md5_file);
  url_lock_.Unlock();

  return ret;
}

int Md5Dedup::SaveYesterdayMd5() {
  string yesterday;
  time_t yesterday_t = time(NULL) - 3600*24;
  Time::GetY4MD2(yesterday_t, &yesterday);
  string md5_file = p_config_->Md5Path() + "/" + yesterday;
  UrlMd5Set *p_yesterday = p_set_list_[1];

  int ret = SaveMd5File(p_yesterday, md5_file);

  return ret;
}

int Md5Dedup::SaveMd5File(UrlMd5Set *p_md5_set, const string &output_file) {
  MD5Generator md5_generator;
  string md5_sum;

  FILE *fp = fopen(output_file.c_str(), "w");
  if (!fp) {
    WriteLog(kLogFatal, "open:%s failed:%s", output_file.c_str(), strerror(errno));
    return -1;
  }

  if (p_md5_set) {
    UrlMd5Set &md5_set = *p_md5_set;
    for (UrlMd5Set::iterator it = md5_set.begin();
        it != md5_set.end(); ++it) {
      const UrlMd5 &url_md5 = *it;
      const unsigned char *m = url_md5.m;
      md5_generator.Digest(m);
      md5_generator.ToString(&md5_sum);
      /// md5sum \t HHMMSS
      fprintf(fp, "%s\t%02d%02d%02d\n",
              md5_sum.c_str(),
              url_md5.hour,
              url_md5.minute,
              url_md5.sec);
    }
  }
  fclose(fp);

  return 0;
}

void *Md5Dedup::TimerThread(void *arg) {
  Md5Dedup *p_dedup = reinterpret_cast<Md5Dedup *>(arg);

  while (true) {
    Sleep::DoSleep(60 * 1000);

    string today;
    Time::GetY4MD2(time(NULL), &today);

    string time_str;
    Time::GetY4MDHMS2(time(NULL), &time_str);
    string hour = time_str.substr(8, 2);

    if (today != p_dedup->today_) {
      /// day switch
      int ret = p_dedup->SwitchDay();
      if (ret < 0)
        WriteLog(kLogWarning, "SwitchDay() failure, today[%s]!=[%s]", today.c_str(), p_dedup->today_.c_str());
      else
        WriteLog(kLogWarning, "SwitchDay() OK, today[%s]!=[%s]", today.c_str(), p_dedup->today_.c_str());
      p_dedup->today_ = today;
    } else if (hour != p_dedup->hour_) {
      /// day hour
      int ret = p_dedup->SwitchHour();
      if (ret < 0)
        WriteLog(kLogWarning, "SwitchHour() failed, hour[%s]!=[%s]", hour.c_str(), p_dedup->hour_.c_str());
      else
        WriteLog(kLogWarning, "SwitchHour() OK, hour[%s]!=[%s]", hour.c_str(), p_dedup->hour_.c_str());
      p_dedup->hour_ = hour;
    }

    int mb = 0;
    if (System::GetMem(&mb) < 0)
      continue;

    int url_count = 0;
    p_dedup->url_lock_.RdLock();
    for (int i = 0; i < p_dedup->day_count_; i++) {
      int item_count = 0;
      if (p_dedup->p_set_list_[i])
        item_count = p_dedup->p_set_list_[i]->size();
      url_count += item_count;
    }
    p_dedup->url_lock_.Unlock();
    WriteLog(kLogNotice, "#url:%d mem:%dMB", url_count, mb);
  }

  return NULL;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

