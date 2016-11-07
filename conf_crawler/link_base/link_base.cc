/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_base.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/link_base/link_base.h"

#include "ganji/util/db/db_pool.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/text/text.h"
#include "ganji/util/file/file.h"
#include "ganji/util/time/time.h"
#include "ganji/util/net/http_opt.h"
#include "ganji/util/log/thread_fast_log.h"
#include "ganji/util/compress/bzip2.h"

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
namespace Bzip2 = ::ganji::util::compress::Bzip2;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;
using ::ganji::util::db::DBPool;
using ::ganji::util::db::SqlResult;

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
int LinkBase::LoadSeedById(int id, bool is_add_link) {
  string load_sql = string(kSelectSqlPrefix) +
                    string(kSeedTable) +
                    " where id = " +
                    Text::IntToStr(id);
  list<string> erase_list, update_list;
  int ret = LoadSeedFromDb(load_sql, is_add_link, &erase_list, &update_list);
  if (ret < 0)
    return -1;

  /// output load info
  WriteLog(kLogNotice, "LoadSeedById[%d] erased:%lu OK", id, erase_list.size());
  for (list<string>::iterator it = erase_list.begin();
      it != erase_list.end(); it++) {
    WriteLog(kLogNotice, "erase:%s", it->c_str());
  }
  int update_count = update_list.size();
  WriteLog(kLogNotice, "LoadSeedById[%d] update:%d OK", id, update_count);
  for (list<string>::iterator it = update_list.begin();
      it != update_list.end(); it++) {
    WriteLog(kLogNotice, "update:%s", it->c_str());
  }

  return 0;
}

int LinkBase::LoadSeedByUrl(const string &seed_url, bool is_add_link) {
  string load_sql = string(kSelectSqlPrefix) +
                    string(kSeedTable) +
                    " where seed_url like '" +
                    seed_url +
                    "'";
  list<string> erase_list, update_list;
  int ret = LoadSeedFromDb(load_sql, is_add_link, &erase_list, &update_list);
  if (ret < 0)
    return -1;

  /// output load info
  WriteLog(kLogNotice, "LoadSeedByUrl[%s] erased:%lu OK", seed_url.c_str(), erase_list.size());
  for (list<string>::iterator it = erase_list.begin();
      it != erase_list.end(); it++) {
    WriteLog(kLogNotice, "erase:%s", it->c_str());
  }
  int update_count = update_list.size();
  WriteLog(kLogNotice, "LoadSeedByUrl[%s] update:%d OK", seed_url.c_str(), update_count);
  for (list<string>::iterator it = update_list.begin();
      it != update_list.end(); it++) {
    WriteLog(kLogNotice, "update:%s", it->c_str());
  }

  return 0;
}

/// load from db, and push into expand_link_queue_
int LinkBase::LoadSeedFromDb(const string &load_sql, bool is_add_link, list<string> *p_erase_list, list<string> *p_update_list) {
  DBPool *p_db_pool = new DBPool(1);
  SqlResult sql_result;
  if (NULL == p_db_pool) {
    WriteLog(kLogWarning, "new DBPool failed");
    return -1;
  }

  int ret = 0;
  do {
    if (!p_db_pool->Initialize(p_config_->GetDbHost(),
                               p_config_->GetDbPort(),
                               p_config_->GetDbUser(),
                               p_config_->GetDbPasswd(),
                               p_config_->GetDbDatabase())) {
      WriteLog(kLogWarning, "DBPool Initialize failed");
      break;
    }

    ret = p_db_pool->Query(load_sql, &sql_result);
    if (ret < 0) {
      WriteLog(kLogWarning, "Query:%s failed", load_sql.c_str());
      break;
    }
  } while (0);

  delete p_db_pool;
  if (ret < 0)
    return -1;

  vector<vector<string> > &vv_record = sql_result.vec_record;
  time_t now = time(NULL);
  seed_item_lock_.WrLock();
  for (vector<vector<string> >::iterator it = vv_record.begin();
      it != vv_record.end(); ++it) {
    vector<string> &record_list = *it;
    SeedItem seed_item;
    seed_item.seed_url_ = record_list[0];
    seed_item.seed_url_ = Http::EscapeURL(seed_item.seed_url_);
    seed_item.is_valid_ = atoi(record_list[1].c_str());
    string downloader_type = record_list[2];
    string header_fields_type = record_list[3];
    string template_type = record_list[4];
    seed_item.url_template_ = record_list[5];
    seed_item.max_depth_ = atoi(record_list[6].c_str());
    seed_item.store_body_depth_ = atoi(record_list[7].c_str());
    seed_item.store_extract_depth_ = atoi(record_list[8].c_str());
    //modified by wangsj
    //seed_item.down_interval_ = atoi(record_list[9].c_str());
    string down_interval_ = record_list[9];
    //end
    seed_item.freq_ = atoi(record_list[10].c_str());
    seed_item.init_freq_ = atoi(record_list[11].c_str());
    seed_item.max_freq_ = atoi(record_list[12].c_str());
    seed_item.min_freq_ = atoi(record_list[13].c_str());
    seed_item.dynamic_page_turn_ = atoi(record_list[14].c_str());
    seed_item.max_pages_ = atoi(record_list[15].c_str());
    seed_item.prev_time_ = now;
    seed_item.next_time_ = now + seed_item.freq_;

    ///add by wangsj
    //parse interval time
    if(LinkUtil::ParseDownIntervalTypeList(down_interval_, &seed_item) < 0) {
      WriteLog(kLogWarning, "seed[%s] invalid  down_interval[%s]",seed_item.seed_url_.c_str(), down_interval_.c_str());
      continue;
    }
    //end

    /// parse downloader type
    if (LinkUtil::ParseDownloaderTypeList(downloader_type, &seed_item) < 0) {
      WriteLog(kLogWarning, "seed[%s] invalid downloader_type[%s]", seed_item.seed_url_.c_str(), downloader_type.c_str());
      continue;
    }

    /// parse header fields type
    if (LinkUtil::ParseHeaderFieldsTypeList(header_fields_type, &seed_item) < 0) {
      WriteLog(kLogWarning, "seed[%s] invalid header_fields_type[%s]", seed_item.seed_url_.c_str(), header_fields_type.c_str());
      continue;
    }

    /// parse extract type, after max_depth_ is set
    if (LinkUtil::ParseTemplateTypeList(template_type, &seed_item) < 0) {
      WriteLog(kLogWarning, "seed[%s] invalid template_type[%s]", seed_item.seed_url_.c_str(), template_type.c_str());
      continue;
    }

    if (seed_item.is_valid_ == 0) {
      SeedItemMap::iterator it_seed = seed_item_map_.find(seed_item.seed_url_);
      if (it_seed != seed_item_map_.end()) {
        it_seed->second.is_valid_ = seed_item.is_valid_;
        p_erase_list->push_back(seed_item.seed_url_);
        seed_item_map_.erase(it_seed);
      }
    } else {
      p_update_list->push_back(seed_item.seed_url_);
      seed_item_map_[seed_item.seed_url_] = seed_item;

      /// push into expand_link_queue_
      if (is_add_link) {
        ExpandLinkItem expand_link_item;
        expand_link_item.url_ = seed_item.seed_url_;
        expand_link_item.seed_url_ = seed_item.seed_url_;
        expand_link_item.depth_ = 0;
        expand_link_item.add_time_ = time(NULL);

        link_queue_lock_.Lock();
        expand_link_queue_.push(expand_link_item);
        link_queue_lock_.Unlock();
        link_queue_cond_.Signal();
      }
    }
  }
  seed_item_lock_.Unlock();

  return 0;
}

/// called by OnGetDownloadTask()
int LinkBase::FillDownloadTaskBySeed(const string &seed_url, int depth, DownloadTask *p_task) {
  SeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    const SeedItem &seed_item = it->second;
    p_task->req_item.downloader_type = seed_item.downloader_type_list_[depth];
    p_task->req_item.header_fields_type = seed_item.header_fields_type_list_[depth];
    //modified by wangsj
    //p_task->prop_item.interval = seed_item.down_interval_;
    p_task->prop_item.interval = seed_item.down_interval_[depth];
    //end
    p_task->prop_item.is_friendly = seed_item.is_friendly_;
    bool is_valid = seed_item.is_valid_;
    if (!is_valid)
      return -1;
  } else {
    return -1;
  }

  return 0;
}

/// store body, and push the item to extract_req_queue_
void LinkBase::ProcUploadBody(const DownloadedBodyItem &downloaded_body_item) {
  const DownloadReqItem &req_item = downloaded_body_item.req_item;
  const DownloadPropItem &prop_item = downloaded_body_item.prop_item;
  const string &url = req_item.url;
  const string &referer = req_item.referer;
  const string &seed_url = prop_item.seed_url;
  bool is_img = prop_item.is_img;
  int depth = prop_item.depth;
  const string &body = downloaded_body_item.body;

  string url_template;
  int max_depth = 0;
  int store_body_depth = 0;
  int store_extract_depth = 0;

  bool found = true;
  bool is_valid = true;
  TemplateType::type template_type;
  seed_item_lock_.RdLock();
  SeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    const SeedItem &seed_item = it->second;
    url_template = seed_item.url_template_;
    max_depth = seed_item.max_depth_;
    store_body_depth = seed_item.store_body_depth_;
    store_extract_depth = seed_item.store_extract_depth_;
    is_valid = seed_item.is_valid_;
    template_type = seed_item.template_type_list_[depth];
  } else {
    found = false;
  }
  seed_item_lock_.Unlock();

  if (!found) {
    WriteLog(kLogFatal, "no seed:%s in seed map", seed_url.c_str());
    return;
  }
  if (!is_valid) {
    WriteLog(kLogFatal, "seed:%s invalid", seed_url.c_str());
    return;
  }

  /// store body
  if (is_img) {
    string body_base64;
    base64_.EncodeBase64(body, &body_base64);
    int ret = mongo_img_.StoreBody(url, seed_url, referer, depth, body_base64);
    int event = kLogDebug;
    string status = "OK";
    if (ret < 0) {
      event = kLogFatal;
      status = "failure";
    }
    WriteLog(event, "Store img[%s] body size[%lu,%lu] %s", url.c_str(), body.size(), body_base64.size(), status.c_str());
    /// XXX trick, for img, return immediately not to extract the body
    return;
  } else if (depth >= store_body_depth) {
    size_t bzip2_len = Bzip2::CompressedLen(body.size());
    char *bzip2_buf = new char[bzip2_len];
    /// bzip2 and encoding with base64
    int ret = Bzip2::Compress(body, bzip2_buf, &bzip2_len);
    if (ret < 0) {
      WriteLog(kLogFatal, "Compress[%s] failed", url.c_str());
    } else {
      string body_base64;
      base64_.EncodeBase64(bzip2_buf, bzip2_len, &body_base64);
      int ret = mongo_body_.StoreBody(url, seed_url, referer, depth, body_base64);
      int event = kLogDebug;
      string status = "OK";
      if (ret < 0) {
        event = kLogFatal;
        status = "failure";
      }
      WriteLog(event, "Store url[%s] body size[%lu,%lu,%lu] %s", url.c_str(), body.size(), bzip2_len, body_base64.size(), status.c_str());
    }
    delete []bzip2_buf;
  }

  /// no need to extract
  if (depth > max_depth)
    return;

  /// push item to extract_req_queue_
  ExtractItem extract_item;
  extract_item.url = url;
  extract_item.url_template = url_template;
  extract_item.depth = depth;
  extract_item.body = body;
  extract_item.seed_url = seed_url;
  extract_item.referer = referer;
  extract_item.template_type = template_type;

  extract_req_lock_.Lock();
  extract_req_queue_.push(extract_item);
  extract_req_lock_.Unlock();
  extract_req_cond_.Signal();
}

/// store fields extracted, and push into expand_link_queue_, called by UploadExtractThreadFunc()
int LinkBase::ProcUploadExtract(const ExtractResultItem &extract_result_item) {
  const ExtractItem &extract_item = extract_result_item.extract_item_;
  const MatchedResultItem &matched_result_item = extract_result_item.matched_result_item_;
  bool is_ok = matched_result_item.is_ok;

  const string &url = extract_item.url;
  const string &seed_url = extract_item.seed_url;
  const string &referer = extract_item.referer;
  int depth = extract_item.depth;
  int max_depth = 0;
  int store_extract_depth = 0;
  int page_turn = 0;
  int prev_link_count = 0;
  SeedItem *pseed_item = NULL;

  if (!is_ok) {
    WriteLog(kLogFatal, "Extract[%s] failed:%s", url.c_str(), matched_result_item.err_info.c_str());
  } else {
    WriteLog(kLogDebug, "Extract[%s] OK", url.c_str());
  }

  bool found = true;
  bool is_valid = true;
  seed_item_lock_.RdLock();
  SeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    SeedItem &seed_item = it->second;
    pseed_item = &seed_item;
    store_extract_depth = seed_item.store_extract_depth_;
    max_depth = seed_item.max_depth_;
    is_valid = seed_item.is_valid_;
    page_turn = seed_item.dynamic_page_turn_;
    prev_link_count=seed_item.link_count_[kCrawlSegCount-1];
  } else {
    found = false;
  }
  seed_item_lock_.Unlock();

  if (!found) {
    WriteLog(kLogFatal, "no seed:%s in seed map", seed_url.c_str());
    return -1;
  }
  if (!is_valid) {
    WriteLog(kLogFatal, "seed:%s invalid", seed_url.c_str());
    return -1;
  }
  
  if (depth == 0 && IsBanned(matched_result_item.self_result))
  {
    int next_time = 0;
    if (seed_url == url)
    {
      seed_item_lock_.WrLock();
      if (pseed_item != NULL) {
        int tmp = pseed_item->freq_ - pseed_item->init_freq_;
        if (tmp < 0)
        {
          tmp = -tmp;
        }
        pseed_item->next_time_ -= tmp;
        next_time = pseed_item->next_time_;
      }
      seed_item_lock_.Unlock();
      WriteLog(kLogDebug, "url[%s] was banned reset next refresh time[%d]", url.c_str(),next_time); 
    }
    else
    {
      ExpandLinkItem expand_link_item;
      expand_link_item.url_ = url;
      expand_link_item.seed_url_ = seed_url;
      expand_link_item.depth_ = depth;
      expand_link_item.referer_ = referer;
      expand_link_item.add_time_ = time(NULL);

      delay_link_queue_lock_.Lock();
      delay_expand_link_queue_.push(expand_link_item);
      delay_link_queue_lock_.Unlock();
      WriteLog(kLogDebug, "Page url[%s] was banned add to delay task list", url.c_str()); 
    }
    return 0;
  }

  /// store fields extracted
  if ((!matched_result_item.self_result.empty() || !matched_result_item.sub_result_list.empty()) &&
      depth >= store_extract_depth) {
    int ret = mongo_extract_.StoreExtract(url, seed_url, referer, depth, matched_result_item);
    int event = kLogDebug;
    string status = "OK";
    if (ret < 0) {
      event = kLogFatal;
      status = "failure";
    }
    WriteLog(event, "Store extract[%s] %s", url.c_str(), status.c_str());
  }

  /// no need to expand link
  if (depth > 0 && depth > max_depth)
    return 0;

  /// expand links by this page
  list<ExpandLinkItem> next_link_item_list;
  list<ExpandLinkItem> next_page_item_list;
  ExpandLinkByMatchedResult(extract_item, matched_result_item, &next_link_item_list,&next_page_item_list);

  /// dedup
  int new_link_count = 0;
  Dedup(max_depth, next_link_item_list, &new_link_count);

  /// next_page
  if (page_turn != 0 && depth == 0 && next_page_item_list.size() > 0 && 
      (next_link_item_list.size() == 0 ||
        (next_link_item_list.size() > 0 &&  new_link_count > 0)))
  {
    bool isExpand = true;

    seed_item_lock_.WrLock();
    if ((pseed_item->pages == 0 && seed_url==url)||
        (pseed_item->lasturl == referer && pseed_item->pages < pseed_item->max_pages_))
    {
      pseed_item->lasturl=url;
      ++pseed_item->pages;
    }
    else if(pseed_item->lasturl == referer && pseed_item->pages >= pseed_item->max_pages_)
    {
      isExpand = false;
      WriteLog(kLogDebug, "Expand over max pages[%d], referer[%s] new_link_count[%d,%d]",pseed_item->max_pages_,url.c_str(),prev_link_count,new_link_count);
    }
    else if(pseed_item->pages == 0 && seed_url != url)
    {
      isExpand = false;
    }
    seed_item_lock_.Unlock();

    if (isExpand)
    {
      for (list<ExpandLinkItem>::const_iterator it = next_page_item_list.begin(); it != next_page_item_list.end(); ++ it)
      {
        const ExpandLinkItem &next_link_item = *it;
        link_queue_lock_.Lock();
        expand_link_queue_.push(next_link_item);
        link_queue_lock_.Unlock();
        link_queue_cond_.Signal();
        WriteLog(kLogDebug, "Expand next page[%s] seed[%s] referer[%s] new_link_count[%d,%d]", next_link_item.url_.c_str(), next_link_item.seed_url_.c_str(), next_link_item.referer_.c_str(),prev_link_count,new_link_count);
      }
    }
  }

  /// adjust frequency for seed url
  if (depth == 0 && seed_url == url) {
    AdjustSeedFreq(seed_url, new_link_count);
  }

  return 0;
}

void LinkBase::TimerThreadFunc() {
  int check_interval = p_config_->GetCheckInterval();
  int save_db_interval = p_config_->SaveDbInterval();
  time_t last_show_time = time(NULL);
  time_t last_save_db_time = time(NULL);

  bool is_night_day_processed = false;
  while (true) {
    Sleep::DoSleep(check_interval * 1000);
    time_t now = time(NULL);
    int show_diff = now - last_show_time;
    int save_db_diff = now - last_save_db_time;

    /// get seed link from seed map
    RefreshSeeds();

    // refresh delay task 
    RefreshDelayTask();

    const string &night_end = p_config_->NightEnd();
    string hm;
    Time::GetY4MDHMS2(now, &hm);
    hm = hm.substr(8, 4);

    if (hm == night_end) {
      if (!is_night_day_processed) {
        ProcessNightDayPoint();
        WriteLog(kLogNotice, "ProcessNightDayPoint()");
        is_night_day_processed = true;
      }
    } else if (hm > night_end) {
      is_night_day_processed = false;
    }

    /// output size of data structure
    if (show_diff >= 60) {
      last_show_time = now;

      link_queue_lock_.Lock();
      int link_count = expand_link_queue_.size();
      link_queue_lock_.Unlock();

      downloaded_lock_.Lock();
      int downloaded_count = downloaded_queue_.size();
      downloaded_lock_.Unlock();

      extract_req_lock_.Lock();
      int extract_req_count = extract_req_queue_.size();
      extract_req_lock_.Unlock();

      extracted_lock_.Lock();
      int extracted_count = extracted_queue_.size();
      extracted_lock_.Unlock();

      WriteLog(kLogNotice, "#link:%d #downloaded:%d #extract req:%d #extracted:%d",
          link_count,
          downloaded_count,
          extract_req_count,
          extracted_count);
    }

    /// save seed info into db
    if (save_db_diff >= save_db_interval) {
      last_save_db_time = now;
      SaveSeedInfoIntoDb();
    }
  }
}

void LinkBase::RefreshSeeds() {
  time_t now = time(NULL);
  seed_item_lock_.WrLock();
  for (SeedItemMap::iterator it = seed_item_map_.begin();
      it != seed_item_map_.end(); ++it) {
    SeedItem &seed_item = it->second;
    time_t next_time = seed_item.next_time_;
    if (next_time > now)
      continue;

    const string &url = it->first;

    string prev_time_str, next_time_str;
    Time::GetY4MDHMS2(seed_item.prev_time_, &prev_time_str);
    prev_time_str = prev_time_str.substr(8);
    Time::GetY4MDHMS2(seed_item.next_time_, &next_time_str);
    next_time_str = next_time_str.substr(8);
    WriteLog(kLogDebug, "Refresh seed[%s] prev time[%s] cur time[%s] freq[%d]",
        url.c_str(), prev_time_str.c_str(), next_time_str.c_str(), seed_item.freq_);

    /// XXX update next_time_
    seed_item.next_time_ = next_time + seed_item.freq_;
    seed_item.prev_time_ = next_time;

    ExpandLinkItem expand_link_item;
    expand_link_item.url_ = url;
    expand_link_item.seed_url_ = seed_item.seed_url_;
    expand_link_item.depth_ = 0;
    expand_link_item.add_time_ = time(NULL);

    link_queue_lock_.Lock();
    expand_link_queue_.push(expand_link_item);
    link_queue_lock_.Unlock();
    link_queue_cond_.Signal();
  }
  seed_item_lock_.Unlock();
}

void LinkBase::RefreshDelayTask() {
  time_t now = time(NULL);
  time_t expire_time = now - 60;
  delay_link_queue_lock_.Lock();
  while(!delay_expand_link_queue_.empty())
  {

    ExpandLinkItem expand_link_item = delay_expand_link_queue_.front();
    if (expand_link_item.add_time_ > expire_time)
    {
      break;
    }
    delay_expand_link_queue_.pop();
    expand_link_item.add_time_ = time(NULL);

    link_queue_lock_.Lock();
    expand_link_queue_.push(expand_link_item);
    link_queue_lock_.Unlock();
    link_queue_cond_.Signal();

    WriteLog(kLogDebug, "Readd banned url[%s]  add task from delay task list", expand_link_item.url_.c_str()); 
  }
  delay_link_queue_lock_.Unlock();
}

void LinkBase::ProcessNightDayPoint() {
  seed_item_lock_.WrLock();
  for (SeedItemMap::iterator it = seed_item_map_.begin();
      it != seed_item_map_.end(); ++it) {
    SeedItem &seed_item = it->second;
    int init_freq = seed_item.init_freq_;
    int &freq = seed_item.freq_;
    if (freq > init_freq)
      freq = init_freq;
    time_t next_time = seed_item.next_time_;

    /// XXX update next_time_
    //seed_item.next_time_ = next_time + seed_item.freq_;
  }
  seed_item_lock_.Unlock();
}

bool LinkBase::IsNight() {
  const string &night_start = p_config_->NightStart();
  const string &night_end = p_config_->NightEnd();
  string hm;
  time_t now = time(NULL);
  Time::GetY4MDHMS2(now, &hm);
  hm = hm.substr(8, 4);

  if (hm >= night_start && hm <= night_end)
    return true;
  else
    return false;
}

void LinkBase::AdjustSeedFreq(const string &seed_url, int new_link_count) {
  float freq_dull_rate = p_config_->FreqDullRate();
  float freq_dull_dull_rate_ = p_config_->FreqDullDullRate();
  float freq_night_contrib_rate_ = p_config_->FreqNightContribRate();
  float freq_incr_rate_ = p_config_->FreqIncrRate();
  float freq_decr_rate_ = p_config_->FreqDecrRate();

  bool is_night = IsNight();
  seed_item_lock_.WrLock();
  SeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    SeedItem &seed_item = it->second;
    int *p_link_count = seed_item.link_count_;
    int &freq = seed_item.freq_;
    int orig_freq = freq;
    int prev_link_count = p_link_count[kCrawlSegCount-1];
    int pp_link_count = p_link_count[kCrawlSegCount-2];

    /// frequency change rate
    float rate = 1.0;
    if (new_link_count == 0) {
      rate = freq_dull_rate;
      if (prev_link_count == 0 || pp_link_count == 0) {
        rate = freq_dull_dull_rate_;
      }
      if (is_night) {
        rate += freq_night_contrib_rate_;
      }
    } else {
      if (new_link_count < prev_link_count) {
        rate = freq_incr_rate_;
        if (is_night) {
          rate -= freq_night_contrib_rate_;
        }
      } else if (new_link_count > prev_link_count) {
        rate = freq_decr_rate_;
        if (is_night) {
          rate += freq_night_contrib_rate_;
        }
      }
    }

    freq = static_cast<int>(static_cast<float>(freq) * rate);
    int max_freq = seed_item.max_freq_, min_freq = seed_item.min_freq_;
    if (freq > max_freq)
      freq = max_freq;
    else if (freq < min_freq)
      freq = min_freq;

    /// update new_link_count into link_count_
    string link_count_str;
    for (int i = 0; i < kCrawlSegCount-1; i++) {
      p_link_count[i] = p_link_count[i+1];
      link_count_str += Text::IntToStr(p_link_count[i]) + ",";
    }
    p_link_count[kCrawlSegCount-1] = new_link_count;
    link_count_str += Text::IntToStr(p_link_count[kCrawlSegCount-1]);

    WriteLog(kLogDebug, "Adjust seed[%s] freq[%d=>%d] #link[%s]",
        seed_url.c_str(),
        orig_freq,
        freq,
        link_count_str.c_str());
  } else {
    WriteLog(kLogFatal, "no seed:%s in seed map", seed_url.c_str());
  }
  seed_item_lock_.Unlock();
}

int LinkBase::SaveSeedInfoIntoDb() {
  vector<string> sql_list;
  seed_item_lock_.RdLock();
  for (SeedItemMap::iterator it = seed_item_map_.begin();
      it != seed_item_map_.end(); ++it) {
    SeedItem &seed_item = it->second;
    const string &seed_url = seed_item.seed_url_;
    int &freq = seed_item.freq_;
    string update_sql = "update " + string(kSeedTable) + " set freq = " + Text::IntToStr(freq) +
      " where seed_url = '" + seed_url + "'";
    sql_list.push_back(update_sql);
  }
  seed_item_lock_.Unlock();

  DBPool *p_db_pool = new DBPool(1);
  if (NULL == p_db_pool) {
    WriteLog(kLogWarning, "SaveSeedInfoIntoDb, new DBPool failed");
    return -1;
  }

  int ret = 0;
  do {
    if (!p_db_pool->Initialize(p_config_->GetDbHost(),
          p_config_->GetDbPort(),
          p_config_->GetDbUser(),
          p_config_->GetDbPasswd(),
          p_config_->GetDbDatabase())) {
      WriteLog(kLogWarning, "SaveSeedInfoIntoDb, DBPool Initialize failed");
      break;
    }

    ret = p_db_pool->ExecuteBatch(sql_list);
    if (ret < 0) {
      WriteLog(kLogWarning, "SaveIntoDb failed, #sql:%ld", sql_list.size());
      break;
    } else {
      WriteLog(kLogNotice, "SaveIntoDb OK, #sql:%ld", sql_list.size());
    }
  } while (0);

  delete p_db_pool;

  return ret;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

