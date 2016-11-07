/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/static_link_base.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/link_base/static_link_base.h"

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
/// load from db, and push into expand_link_queue_
int StaticLinkBase::LoadDbTask(int task_id, bool is_add_task) {
  DBPool *p_db_pool = new DBPool(1);
  SqlResult sql_result;
  if (NULL == p_db_pool) {
    WriteLog(kLogWarning, "new DBPool failed");
    return -1;
  }

  string load_sql = "select seed_url, is_valid, downloader_type, header_fields_type, template_type, url_template, max_depth, store_body_depth, store_extract_depth, down_interval from " +
                     string(kStaticTable) +
                     " where task_id = " +
                     Text::IntToStr(task_id);
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
  } while (false);

  delete p_db_pool;
  if (ret < 0)
    return -1;

  vector<vector<string> > &vv_record = sql_result.vec_record;
  list<string> erase_list, update_list;
  seed_item_lock_.WrLock();
  for (vector<vector<string> >::iterator it = vv_record.begin();
      it != vv_record.end(); ++it) {
    vector<string> &record_list = *it;
    StaticSeedItem seed_item;
    seed_item.task_id_ = task_id;
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
    seed_item.down_interval_ = atoi(record_list[9].c_str());

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
      StaticSeedItemMap::iterator it_seed = seed_item_map_.find(seed_item.seed_url_);
      if (it_seed != seed_item_map_.end()) {
        it_seed->second.is_valid_ = seed_item.is_valid_;
        erase_list.push_back(seed_item.seed_url_);
        seed_item_map_.erase(it_seed);
      }
    } else {
      update_list.push_back(seed_item.seed_url_);
      seed_item_map_[seed_item.seed_url_] = seed_item;

      if (is_add_task) {
        /// push into expand_link_queue_
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

  /// output load info
  int update_count = 0;
  if (!vv_record.empty()) {
    WriteLog(kLogNotice, "LoadDbTask[%d] erased:%lu OK", task_id, erase_list.size());
    for (list<string>::iterator it = erase_list.begin();
        it != erase_list.end(); it++) {
      WriteLog(kLogNotice, "erase:%s", it->c_str());
    }
    update_count = update_list.size();
    WriteLog(kLogNotice, "LoadDbTask[%d] update:%d OK", task_id, update_count);
    for (list<string>::iterator it = update_list.begin();
        it != update_list.end(); it++) {
      WriteLog(kLogNotice, "update:%s", it->c_str());
    }
  }

  return 0;
}

/// load from mongodb, and push into expand_link_queue_
int StaticLinkBase::LoadMongoDbTask(int task_id, bool is_add_task) {
  list<StaticSeedItem> seed_list;
  int ret = mongo_seed_.QueryStaticSeeds(task_id, &seed_list);
  if (ret < 0)
    return -1;

  list<string> erase_list, update_list;

  seed_item_lock_.WrLock();
  for (list<StaticSeedItem>::iterator it = seed_list.begin();
      it != seed_list.end(); ++it) {
    const StaticSeedItem &seed_item = *it;

    if (seed_item.is_valid_ == 0) {
      StaticSeedItemMap::iterator it_seed = seed_item_map_.find(seed_item.seed_url_);
      if (it_seed != seed_item_map_.end()) {
        it_seed->second.is_valid_ = seed_item.is_valid_;
        erase_list.push_back(seed_item.seed_url_);
        // seed_item_map_.erase(it_seed);
      }
    } else {
      update_list.push_back(seed_item.seed_url_);
      seed_item_map_[seed_item.seed_url_] = seed_item;

      if (is_add_task) {
        /// push into expand_link_queue_
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

  /// output load info
  WriteLog(kLogNotice, "LoadMongoDbTask[%d] erased:%lu OK", task_id, erase_list.size());
  for (list<string>::iterator it = erase_list.begin();
      it != erase_list.end(); it++) {
    WriteLog(kLogNotice, "erase:%s", it->c_str());
  }
  int update_count = update_list.size();
  WriteLog(kLogNotice, "LoadMongoDbTask[%d] update:%d OK", task_id, update_count);
  for (list<string>::iterator it = update_list.begin();
      it != update_list.end(); it++) {
    WriteLog(kLogNotice, "update:%s", it->c_str());
  }

  return 0;
}

int StaticLinkBase::FillDownloadTaskBySeed(const string &seed_url, int depth, DownloadTask *p_task) {
  StaticSeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    const StaticSeedItem &seed_item = it->second;
    p_task->req_item.downloader_type = seed_item.downloader_type_list_[depth];
    p_task->req_item.header_fields_type = seed_item.header_fields_type_list_[depth];
    p_task->prop_item.interval = seed_item.down_interval_;
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
void StaticLinkBase::ProcUploadBody(const DownloadedBodyItem &downloaded_body_item) {
  const DownloadReqItem &req_item = downloaded_body_item.req_item;
  const DownloadPropItem &prop_item = downloaded_body_item.prop_item;
  const string &url = req_item.url;
  const string &referer = req_item.referer;
  const string &seed_url = prop_item.seed_url;
  bool is_img = prop_item.is_img;
  int depth = prop_item.depth;
  const string &body = downloaded_body_item.body;

  string url_template;
  int task_id = -1;
  int max_depth = 0;
  int store_body_depth = 0;
  int store_extract_depth = 0;

  bool found = true;
  bool is_valid = true;
  TemplateType::type template_type;
  seed_item_lock_.RdLock();
  StaticSeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    const StaticSeedItem &seed_item = it->second;
    task_id = seed_item.task_id_;
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
    int ret = mongo_img_.StoreBody(url, seed_url, referer, depth, body_base64, task_id);
    if (ret == 0)
      WriteLog(kLogDebug, "store img[%s] OK", url.c_str());
    else
      WriteLog(kLogDebug, "store img[%s] failed", url.c_str());
    /// XXX trick, for img, return immediately not to extract the body
    return;
  } else if (depth >= store_body_depth) {
    int ret = mongo_body_.StoreBody(url, seed_url, referer, depth, body, task_id);
    if (ret == 0)
      WriteLog(kLogDebug, "store body[%s] OK", url.c_str());
    else
      WriteLog(kLogDebug, "store body[%s] failed", url.c_str());
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
int StaticLinkBase::ProcUploadExtract(const ExtractResultItem &extract_result_item) {
  const ExtractItem &extract_item = extract_result_item.extract_item_;
  const MatchedResultItem &matched_result_item = extract_result_item.matched_result_item_;
  bool is_ok = matched_result_item.is_ok;

  const string &url = extract_item.url;
  const string &seed_url = extract_item.seed_url;
  const string &referer = extract_item.referer;
  int task_id = 0;
  int depth = extract_item.depth;
  int max_depth = 0;
  int store_extract_depth = 0;

  if (!is_ok) {
    WriteLog(kLogFatal, "Extract[%s] failed:%s", url.c_str(), matched_result_item.err_info.c_str());
  } else {
    WriteLog(kLogDebug, "Extract[%s] OK", url.c_str());
  }

  bool found = true;
  bool is_valid = true;
  seed_item_lock_.RdLock();
  StaticSeedItemMap::iterator it = seed_item_map_.find(seed_url);
  if (it != seed_item_map_.end()) {
    const StaticSeedItem &seed_item = it->second;
    task_id = seed_item.task_id_;
    store_extract_depth = seed_item.store_extract_depth_;
    max_depth = seed_item.max_depth_;
    is_valid = seed_item.is_valid_;
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

  if (IsBanned(matched_result_item.self_result))
  {
    WriteLog(kLogDebug, "url[%s] was banned ", url.c_str());

    ExpandLinkItem expand_link_item;
    expand_link_item.url_ = url;
    expand_link_item.seed_url_ = seed_url;
    expand_link_item.depth_ = depth;
    expand_link_item.referer_ = referer;
    expand_link_item.add_time_ = time(NULL);

    link_queue_lock_.Lock();
    expand_link_queue_.push(expand_link_item);
    link_queue_lock_.Unlock();
    link_queue_cond_.Signal();

    return 0;
  }

  /// store fields extracted
  if ((!matched_result_item.self_result.empty() || !matched_result_item.sub_result_list.empty()) &&
      depth >= store_extract_depth) {
    int ret = mongo_extract_.StoreExtract(url, seed_url, referer, depth, matched_result_item, task_id);
    if (ret == 0)
      WriteLog(kLogDebug, "store extract[%s] OK", url.c_str());
    else
      WriteLog(kLogDebug, "store extract[%s] failed", url.c_str());
  }

  /// no need to expand link
  if (depth > max_depth)
    return -1;

  /// expand links by this page
  list<ExpandLinkItem> next_link_item_list;
  list<ExpandLinkItem> next_page_item_list;
  ExpandLinkByMatchedResult(extract_item, matched_result_item, &next_link_item_list,&next_page_item_list);

  /// dedup
  int new_link_count = 0;
  Dedup(max_depth, next_link_item_list, &new_link_count);

  for (list<ExpandLinkItem>::const_iterator it = next_page_item_list.begin(); it != next_page_item_list.end(); ++ it)
  {
    const ExpandLinkItem &next_link_item = *it;
    link_queue_lock_.Lock();
    expand_link_queue_.push(next_link_item);
    link_queue_lock_.Unlock();
    link_queue_cond_.Signal();
    WriteLog(kLogDebug, "Expand next page[%s] seed[%s] referer[%s]", next_link_item.url_.c_str(), next_link_item.seed_url_.c_str(), next_link_item.referer_.c_str());
  }

  return 0;
}

void StaticLinkBase::TimerThreadFunc() {
  int check_interval = p_config_->GetCheckInterval();
  Sleep::DoSleep(check_interval * 1000);
  time_t last_show_time = time(NULL);

  while (true) {
    Sleep::DoSleep(1 * 1000);
    time_t now = time(NULL);
    int diff = now - last_show_time;

    /// output size of data structure
    if (diff >= 60) {
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
  }
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

