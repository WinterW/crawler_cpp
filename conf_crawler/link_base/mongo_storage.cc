/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/mongo_storage.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-05
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/link_base/mongo_storage.h"

#include <vector>
#include <map>
#include <list>

#include "conf_crawler/link_base/link_config.h"
#include "conf_crawler/link_base/link_util.h"
#include "conf_crawler/link_base/struct_def.h"
#include "util/log/thread_fast_log.h"
#include "util/time/time.h"
#include "util/text/text.h"
#include "util/net/http_opt.h"
#include "util/thread/sleep.h"

using std::vector;
using std::map;
using std::list;

namespace Sleep = ::ganji::util::thread::Sleep;
namespace Time = ::ganji::util::time;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
namespace Text = ::ganji::util::text::Text;
namespace Http = ::ganji::util::net::Http;
namespace LinkUtil = ::ganji::crawler::conf_crawler::link_base::LinkUtil;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
int MongoBase::Init(LinkConfig *p_config, const string &db_name) {
  p_config_ = p_config;
  db_name_ = db_name;

  try {
    string errmsg;
    /// connect
    const string &mongo_host_port = p_config_->GetMongoHostPort();
    if (!conn_.connect(mongo_host_port, errmsg)) {
      WriteLog(kLogFatal, "connect to mongo:%s failed:%s",
          mongo_host_port.c_str(), errmsg.c_str());
      return -1;
    }
  } catch(const mongo::DBException &e) {
    WriteLog(kLogFatal, "caught:%s", e.toString().c_str());
    return -1;
  }

  /// login
  if (Login(db_name_) < 0)
    return -1;

  return 0;
}

int MongoBase::Login(const string &db) {
  try {
    string errmsg;
    /// authenticate
    const string &user = p_config_->GetMongoUser();
    const string &passwd = p_config_->GetMongoPasswd();
    bool ret = conn_.auth(db, user, passwd, errmsg);
    if (!ret) {
      WriteLog(kLogFatal, "auth[%s] failed:%s", db.c_str(), errmsg.c_str());
      return -1;
    }
  } catch(const mongo::DBException &e) {
    WriteLog(kLogFatal, "caught:%s", e.toString().c_str());
    return -1;
  }

  return 0;
}

int MongoBody::StoreBody(const string &url,
                         const string &seed_url,
                         const string &referer,
                         int depth,
                         const string &body,
                         int task_id) {
  string body_cltn = kMongoBodyCltn;

  string body_db_cltn = db_name_ + "." + body_cltn;
  /// current millisec
  uint64_t cur_time_ms = 0;
  Time::GetCurTimeMs(&cur_time_ms);
  mongo::Date_t ts(cur_time_ms);

  mongo::BSONObjBuilder obj;
  obj.append("url", url);
  obj.append("seed_url", seed_url);
  obj.append("referer", referer);
  obj.append("depth", depth);
  obj.append("body", body);
  if (task_id != -1)
    obj.append("task_id", task_id);
  obj.append("time", ts);
  try {
    conn_.insert(body_db_cltn, obj.obj());
  } catch(const mongo::DBException &e) {
    WriteLog(kLogFatal, "insert body to mongo failed:%s", e.toString().c_str());
    return -1;
  }

  return 0;
}

int MongoExtract::StoreExtract(const string &url,
                               const string &seed_url,
                               const string &referer,
                               int depth,
                               const MatchedResultItem &matched_result_item,
                               int task_id) {
  const string &extract_db = p_config_->MongoExtractDb();
  string extract_cltn = kMongoExtractCltn;

  const map<string, vector<string> > &self_result = matched_result_item.self_result;
  const vector<map<string, vector<string> > > &sub_result_list = matched_result_item.sub_result_list;

  mongo::BSONObjBuilder self_result_obj;
  for (map<string, vector<string> >::const_iterator it = self_result.begin();
      it != self_result.end(); ++it) {
    self_result_obj.append(it->first, it->second);
  }

  mongo::BSONObjBuilder sub_result_list_obj;
  int i = 0;
  mongo::BSONObjBuilder obj;
  for (vector<map<string, vector<string> > >::const_iterator it = sub_result_list.begin();
      it != sub_result_list.end(); ++it) {
    const map<string, vector<string> > &sub_result = *it;
    mongo::BSONObjBuilder sub_result_obj;

    for (map<string, vector<string> >::const_iterator it2 = sub_result.begin();
        it2 != sub_result.end(); ++it2) {
      sub_result_obj.append(it2->first, it2->second);
    }
    string i_str = Text::IntToStr(i++);
    sub_result_list_obj.append(i_str, sub_result_obj.obj());
  }

  obj.append("self_result", self_result_obj.obj());
  obj.append("sub_result_list", sub_result_list_obj.obj());

  /// current millisec
  uint64_t cur_time_ms = 0;
  Time::GetCurTimeMs(&cur_time_ms);
  mongo::Date_t ts(cur_time_ms);

  obj.append("url", url);
  obj.append("seed_url", seed_url);
  obj.append("referer", referer);
  obj.append("depth", depth);
  if (task_id != -1)
    obj.append("task_id", task_id);
  obj.append("time", ts);

  string extract_db_cltn = extract_db + "." + extract_cltn;
  try {
    conn_.insert(extract_db_cltn, obj.obj());
  } catch(const mongo::DBException &e) {
    WriteLog(kLogFatal, "insert extract to mongo failed:%s", e.toString().c_str());
    return -1;
  }

  return 0;
}

int MongoSeed::QueryStaticSeeds(int task_id, list<StaticSeedItem> *p_seed_list) {
  const string &seed_db = p_config_->GetMongoSeedDb();
  string seed_cltn = seed_db + "." + string(kMongoStaticSeedCltn);

  std::auto_ptr<mongo::DBClientCursor> cursor = conn_.query(seed_cltn,
                                                            QUERY("task_id" << task_id));

  try {
    while (cursor->more()) {
      mongo::BSONObj p = cursor->next();
      StaticSeedItem seed_item;
      seed_item.seed_url_ = p.getStringField("seed_url");
      seed_item.url_template_ = p.getStringField("url_template");
      string update_time = p.getStringField("update_time");
      int is_valid = p.getIntField("is_valid");
      int max_depth = p.getIntField("max_depth");
      int store_body_depth = p.getIntField("store_body_depth");
      int store_extract_depth = p.getIntField("store_extract_depth");
      int is_friendly = p.getIntField("is_friendly");
      int down_interval = p.getIntField("down_interval");

      if (seed_item.seed_url_.empty() ||
          seed_item.url_template_.empty() ||
          is_valid == INT_MIN ||
          max_depth == INT_MIN ||
          store_body_depth == INT_MIN ||
          store_extract_depth == INT_MIN ||
          is_friendly == INT_MIN ||
          down_interval == INT_MIN) {
        WriteLog(kLogWarning, "mongodb segment for task_id[%d] invalid", task_id);
        continue;
      }

      seed_item.seed_url_ = Http::EscapeURL(seed_item.seed_url_);
      seed_item.is_valid_ = is_valid;
      seed_item.max_depth_ = max_depth;
      seed_item.store_body_depth_ = store_body_depth;
      seed_item.store_extract_depth_ = store_extract_depth;

      /// parse extract type, after max_depth_ is set
      string template_type = p.getStringField("template_type");
      if (LinkUtil::ParseTemplateTypeList(template_type, &seed_item) < 0) {
        WriteLog(kLogWarning, "mongo task_id[%d] seed[%s] invalid template_type:%s", task_id, seed_item.seed_url_.c_str(), template_type.c_str());
        continue;
      }
      if (is_friendly == 1)
        seed_item.is_friendly_ = true;
      else
        seed_item.is_friendly_ = false;
      seed_item.down_interval_ = down_interval;
      p_seed_list->push_back(seed_item);
    }
  } catch(const mongo::DBException &e) {
    WriteLog(kLogFatal, "QueryStaticSeeds failed:%s", e.toString().c_str());
    return -1;
  }

  if (p_seed_list->empty()) {
    WriteLog(kLogFatal, "QueryStaticSeeds task_id[%d] empty", task_id);
    return -1;
  }

  return 0;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

