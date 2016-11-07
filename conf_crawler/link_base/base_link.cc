/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/base_link.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-19
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/link_base/base_link.h"

#include "ganji/util/db/db_pool.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/text/text.h"
#include "ganji/util/file/file.h"
#include "ganji/util/time/time.h"
#include "ganji/util/net/http_opt.h"
#include "ganji/util/log/thread_fast_log.h"

#include "ganji/crawler/conf_crawler/link_base/link_config.h"
#include "ganji/crawler/conf_crawler/link_base/link_util.h"

using std::list;
using std::string;
using std::vector;

namespace LinkUtil = ::ganji::crawler::conf_crawler::link_base::LinkUtil;
namespace Http = ::ganji::util::net::Http;
namespace Time = ::ganji::util::time;
namespace Text = ::ganji::util::text::Text;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;
using ::ganji::util::db::DBPool;
using ::ganji::util::db::SqlResult;

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
int BaseLink::Init(LinkConfig *p_config) {
  if (!p_config) {
    WriteLog(kLogFatal, "link config NULL");
    return -1;
  }

  p_config_ = p_config;

  mongo_body_.Init(p_config_, p_config_->MongoBodyDb());
  mongo_extract_.Init(p_config_, p_config_->MongoExtractDb());
  mongo_img_.Init(p_config_, p_config_->MongoImgDb());
  mongo_seed_.Init(p_config_, p_config_->GetMongoSeedDb());

  int socket_timeout = p_config_->GetSocketTimeout();
  int persist_count = p_config_->GetPersistCount();
  dedup_conn_.Init(p_config_->GetDedupHost(), p_config_->GetDedupPort(), socket_timeout, persist_count);

  /// start timer thread
  p_timer_thread_ = Thread::CreateThread(TimerThread, this);
  p_timer_thread_->ResumeThread();

  /// start upload body thread
  p_upload_body_thread_ = Thread::CreateThread(UploadBodyThread, this);
  p_upload_body_thread_->ResumeThread();

  /// start upload extract thread
  p_upload_extract_thread_ = Thread::CreateThread(UploadExtractThread, this);
  p_upload_extract_thread_->ResumeThread();

  return 0;
}

/// pop from expand_link_queue_
int BaseLink::OnGetDownloadTask(std::vector<DownloadTask> *p_list) {
  p_list->clear();

  seed_item_lock_.RdLock();
  link_queue_lock_.Lock();
  while (!expand_link_queue_.empty()) {
    const ExpandLinkItem &expand_link_item = expand_link_queue_.front();
    const string &seed_url = expand_link_item.seed_url_;

    DownloadTask download_task;
    download_task.req_item.url = expand_link_item.url_;
    download_task.req_item.referer = expand_link_item.referer_;
    download_task.prop_item.is_img = expand_link_item.is_img_;
    download_task.prop_item.depth = expand_link_item.depth_;
    download_task.prop_item.seed_url = seed_url;

    /// fill download task by seed
    int ret = FillDownloadTaskBySeed(seed_url, download_task.prop_item.depth, &download_task);
    if (ret < 0)
      continue;

    p_list->push_back(download_task);

    WriteLog(kLogDebug, "Dc GetTask[%s]", download_task.req_item.url.c_str());

    expand_link_queue_.pop();
  }
  link_queue_lock_.Unlock();
  seed_item_lock_.Unlock();

  if (p_list->empty()) {
    return -1;
  }

  return 0;
}

/// pop from extract_req_queue_
int BaseLink::OnGetExtractItem(std::vector<ExtractItem> *p_list) {
  p_list->clear();

  extract_req_lock_.Lock();
  while (!extract_req_queue_.empty()) {
    const ExtractItem &extract_item = extract_req_queue_.front();
    p_list->push_back(extract_item);
    WriteLog(kLogDebug, "Extractor GetTask[%s]", extract_item.url.c_str());
    extract_req_queue_.pop();
  }
  extract_req_lock_.Unlock();

  if (p_list->empty()) {
    return -1;
  }

  return 0;
}

/// push into downloaded_queue_
void BaseLink::OnUploadBody(const DownloadedBodyItem &downloaded_body_item) {
  downloaded_lock_.Lock();
  downloaded_queue_.push(downloaded_body_item);
  downloaded_lock_.Unlock();
  downloaded_cond_.Signal();
}

/// push into extracted_queue_
void BaseLink::OnUploadExtract(const ExtractItem &extract_item, const MatchedResultItem &matched_result_item) {
  ExtractResultItem extract_result_item(extract_item, matched_result_item);
  extracted_lock_.Lock();
  extracted_queue_.push(extract_result_item);
  extracted_lock_.Unlock();
  extracted_cond_.Signal();
}

void * BaseLink::TimerThread(void *arg) {
  BaseLink *p_link_base = reinterpret_cast<BaseLink *>(arg);
  p_link_base->TimerThreadFunc();

  return NULL;
}

void * BaseLink::UploadBodyThread(void *arg) {
  BaseLink *p_link_base = reinterpret_cast<BaseLink *>(arg);

  while (true) {
    p_link_base->UploadBodyThreadFunc();
  }

  return NULL;
}

/// pop item from downloaded_queue_, store body, and push it to extract_req_queue_
int BaseLink::UploadBodyThreadFunc() {
  /// pop item from downloaded_queue_
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
  bool is_ok = downloaded_body_item.is_ok;
  if (!is_ok) {
    WriteLog(kLogFatal, "Download[%s] failed by dc", url.c_str());
    return -1;
  }
  WriteLog(kLogDebug, "Download[%s] OK by dc", url.c_str());

  ProcUploadBody(downloaded_body_item);

  return 0;
}

void * BaseLink::UploadExtractThread(void *arg) {
  BaseLink *p_link_base = reinterpret_cast<BaseLink *>(arg);
  int check_interval = p_link_base->p_config_->GetCheckInterval();
  Sleep::DoSleep(check_interval * 1000);

  while (true) {
    p_link_base->UploadExtractThreadFunc();
  }

  return NULL;
}

/// pop from extracted_queue_, store fields extracted, and push into expand_link_queue_
int BaseLink::UploadExtractThreadFunc() {
  /// pop item from extracted_queue_
  extracted_lock_.Lock();
  while (extracted_queue_.empty()) {
    extracted_lock_.Unlock();
    extracted_cond_.Wait();
    extracted_lock_.Lock();
  }
  ExtractResultItem extract_result_item = extracted_queue_.front();
  extracted_queue_.pop();
  extracted_lock_.Unlock();

  /// process extract item uploaded by Extractor
  ProcUploadExtract(extract_result_item);

  return 0;
}

void BaseLink::ExpandLinkByMatchedResult(const ExtractItem &extract_item,
                                         const MatchedResultItem &matched_result_item,
                                         list<ExpandLinkItem> *p_next_link_item_list,
                                         list<ExpandLinkItem> *p_next_page_item_list) {
  ExpandLinkByMap(extract_item, matched_result_item.self_result, p_next_link_item_list,p_next_page_item_list);
  for (vector<map<string, vector<string> > >::const_iterator it = matched_result_item.sub_result_list.begin();
      it != matched_result_item.sub_result_list.end(); ++it) {
    ExpandLinkByMap(extract_item, *it, p_next_link_item_list,p_next_page_item_list);
  }
}

void BaseLink::ExpandLinkByMap(const ExtractItem &extract_item,
                               const map<string, vector<string> > &result_map,
                               list<ExpandLinkItem> *p_next_link_item_list,
                               list<ExpandLinkItem> *p_next_page_item_list) {
  const string &seed_url = extract_item.seed_url;
  const string &referer = extract_item.url;
  int depth = extract_item.depth;
  map<string, vector<string> >::const_iterator it_link_url = result_map.find(kLinkUrl);
  if (it_link_url != result_map.end()) {
    const vector<string> &link_list = it_link_url->second;
    GenerateLink(link_list,
                 seed_url,
                 referer,
                 false,
                 false,
                 depth+1,
                 p_next_link_item_list);
  }

  map<string, vector<string> >::const_iterator it_post_url = result_map.find(kPostUrl);
  if (it_post_url != result_map.end()) {
    const vector<string> &link_list = it_post_url->second;
    GenerateLink(link_list,
                 seed_url,
                 referer,
                 false,
                 true,
                 depth+1,
                 p_next_link_item_list);
  }

  map<string, vector<string> >::const_iterator it_img_url = result_map.find(kImgUrl);
  if (it_img_url != result_map.end()) {
    const vector<string> &link_list = it_img_url->second;
    GenerateLink(link_list,
                 seed_url,
                 referer,
                 true,
                 false,
                 depth+1,
                 p_next_link_item_list);
  }

  map<string, vector<string> >::const_iterator it_phone_img_url = result_map.find(kPhoneImgUrl);
  if (it_phone_img_url != result_map.end()) {
    const vector<string> &link_list = it_phone_img_url->second;
    GenerateLink(link_list,
                 seed_url,
                 referer,
                 true,
                 true,
                 depth+1,
                 p_next_link_item_list);
  }

  map<string, vector<string> >::const_iterator it_next_page = result_map.find(kNextPage);
  if (it_next_page != result_map.end()) {
    const vector<string> &link_list = it_next_page->second;
    /// XXX depth of next page equals to that of current url
    GenerateLink(link_list,
                 seed_url,
                 referer,
                 false,
                 false,
                 depth,
                 p_next_page_item_list);
  }
}

void BaseLink::GenerateLink(const vector<string> &link_list,
                            const string &seed_url,
                            const string &referer,
                            bool is_img_url,
                            bool is_phone_img_url,
                            int depth,
                            list<ExpandLinkItem> *p_next_link_item_list) {
  for (vector<string>::const_iterator it_list = link_list.begin();
      it_list != link_list.end(); ++it_list) {
    const string &link_url = *it_list;
    ExpandLinkItem next_link_item;
    string abs_url;
    if (LinkUtil::Relative2AbsUrl(referer, link_url, &abs_url) < 0) {
      continue;
    }
    if (referer == link_url)//防止下一页等于自己，造成重复抓取
    {
      continue;
    }
    next_link_item.url_ = abs_url;
    next_link_item.seed_url_ = seed_url;
    next_link_item.is_img_ = is_img_url;
    next_link_item.is_phone_img_ = is_phone_img_url;
    next_link_item.referer_ = referer;
    next_link_item.depth_ = depth;
    next_link_item.add_time_ = time(NULL);
    p_next_link_item_list->push_back(next_link_item);
  }
}

/// query dedup svr, push into expand_link_queue_ if not exist
int BaseLink::Dedup(int max_depth, const list<ExpandLinkItem> &next_link_item_list, int *p_new_link_count) {
  *p_new_link_count = 0;
  for (list<ExpandLinkItem>::const_iterator it = next_link_item_list.begin();
      it != next_link_item_list.end(); ++it) {
    const ExpandLinkItem &next_link_item = *it;
    bool is_img = next_link_item.is_img_;
    bool is_phone_img = next_link_item.is_phone_img_;
    int depth = next_link_item.depth_;
    const string &url = next_link_item.url_;
    const string &referer = next_link_item.referer_;
    bool exists = false;

    /// XXX Do not check img
    if (is_img || is_phone_img) {
      exists = false;
      WriteLog(kLogDebug, "NO check Img[%s]", url.c_str());
    } else {
      int ret = TestExistsAndInsert(url, &exists);
      if (ret < 0) {
        WriteLog(kLogNotice, "TestExistsAndInsert:%s failed", url.c_str());
      }
      if (exists)
        continue;
      /// new link
      ++(*p_new_link_count);
    }

    /// remove link whose depth is not needed
    if (!is_img && depth > max_depth) {
      WriteLog(kLogDebug, "drop by Dedup[%s] depth:%d > max_depth:%d", url.c_str(), depth, max_depth);
      continue;
    }

    link_queue_lock_.Lock();
    expand_link_queue_.push(next_link_item);
    link_queue_lock_.Unlock();
    link_queue_cond_.Signal();
    WriteLog(kLogDebug, "Expand by Dedup[%s] seed[%s] referer[%s]", url.c_str(), next_link_item.seed_url_.c_str(), referer.c_str());
  }

  return 0;
}

int BaseLink::TestExistsAndInsert(const string &url, bool *p_exists) {
  *p_exists = false;

  int ret = 0;
  dedup_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (dedup_conn_.NeedReset()) {
      bool is_ok = dedup_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetRetryTimes(); i++) {
      try {
        *p_exists = dedup_conn_.Client()->test_exists_and_insert(url);
        dedup_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(...) {
        ret = -1;
        WriteLog(kLogFatal, "test_exists_and_insert() failed");
        bool is_ok = dedup_conn_.Reset();
        if (!is_ok) {
          break;
        }
      }
    }
  } while (0);
  dedup_conn_.Unlock();

  return ret;
}

bool BaseLink::IsBanned(const map<string, vector<string> > &result_map)
{
  map<string, vector<string> >::const_iterator it_banned = result_map.find(kIsBannedPage);
  if (it_banned != result_map.end()) {
    return true;
  }

  return false;

}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

