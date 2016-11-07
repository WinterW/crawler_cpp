/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/static_dedup.cc
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-31
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/url_dedup/static_dedup.h"

#include <boost/regex.hpp>

#include "util/log/thread_fast_log.h"
#include "util/thread/sleep.h"
#include "util/system/system.h"
#include "conf_crawler/url_dedup/dedup_config.h"

using std::string;

using boost::smatch;
using boost::regex;
using boost::regex_search;
using boost::regex_error;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

using ganji::util::thread::Thread;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace System = ::ganji::util::system::System;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
StaticDedup::~StaticDedup() {
  Thread::FreeThread(p_timer_thread_);
}

int StaticDedup::Init(DedupConfig *p_config) {
  p_config_ = p_config;

  int bucket_count = p_config_->BucketCount();
  url_set_.rehash(bucket_count);

  /// start timer thread
  p_timer_thread_ = Thread::CreateThread(TimerThread, this);
  p_timer_thread_->ResumeThread();

  return 0;
}

bool StaticDedup::IsExists(const string &url) {
  url_lock_.RdLock();
  StringHashSet::iterator it = url_set_.find(url);
  bool is_exists = (it != url_set_.end());
  url_lock_.Unlock();

  return is_exists;
}

bool StaticDedup::Insert(const string &url) {
  url_lock_.WrLock();
  url_set_.insert(url);
  url_lock_.Unlock();

  return true;
}

bool StaticDedup::TestExistsAndInsert(const string &url, bool *p_exists) {
  url_lock_.WrLock();
  StringHashSet::iterator it = url_set_.find(url);
  if (it == url_set_.end()) {
    *p_exists = false;
    url_set_.insert(url);
  } else {
    *p_exists = true;
  }
  url_lock_.Unlock();

  return true;
}

void StaticDedup::Rehash(int bucket_count) {
  if (bucket_count <= 0) {
    WriteLog(kLogWarning, "Rehash(),#bucket:%d invalid", bucket_count);
    return;
  }

  url_lock_.WrLock();
  url_set_.rehash(bucket_count);
  url_lock_.Unlock();
}

bool StaticDedup::Remove(const std::string &url) {
  bool found = true;
  url_lock_.WrLock();
  StringHashSet::iterator it = url_set_.find(url);
  if (it == url_set_.end()) {
    found = false;
  } else {
    url_set_.erase(it);
  }
  url_lock_.Unlock();

  return found;
}

int StaticDedup::BatchRemove(const std::string &url_pattern) {
  int count = 0;

  url_lock_.WrLock();
  for (StringHashSet::iterator it = url_set_.begin();
      it != url_set_.end();) {
    const string &url = *it;

    try {
      smatch what;
      regex url_regex = regex(url_pattern);
      if (regex_search(url.begin(), url.end(), what, url_regex)) {
        url_set_.erase(it++);
        count++;
      } else {
        ++it;
      }
    } catch(const regex_error &e) {
      WriteLog(kLogFatal, "regex error:%s", e.what());
      count = -1;
      break;
    }
  }
  url_lock_.Unlock();

  return count;
}

void *StaticDedup::TimerThread(void *arg) {
  StaticDedup *p_dedup = reinterpret_cast<StaticDedup *>(arg);

  while (true) {
    Sleep::DoSleep(60 * 1000);
    int mb = 0;
    if (System::GetMem(&mb) < 0)
      continue;
    p_dedup->url_lock_.RdLock();
    int url_count = p_dedup->url_set_.size();
    p_dedup->url_lock_.Unlock();
    WriteLog(kLogNotice, "#url:%d mem:%dMB", url_count, mb);
  }

  return NULL;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

