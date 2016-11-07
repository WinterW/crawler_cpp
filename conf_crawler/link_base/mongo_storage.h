/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/mongo_storage.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-05
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_MONGO_STORAGE_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_MONGO_STORAGE_H_

#include <string>

#include "client/dbclient.h"

#include "global.h"
#include "util/thread/thread.h"
#include "util/thread/mutex.h"

#include "conf_crawler/link_base/base64.h"
#include "conf_crawler/link_base/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
class LinkConfig;

using ganji::util::thread::Thread;
using ganji::util::thread::Mutex;

/**
 * @classe MongoBase
 * base class for mongo storage class
 */
class MongoBase {
 public:
  MongoBase()
    : p_config_(NULL),
    conn_(true) {
  }

  ~MongoBase() {
  }

  int Init(LinkConfig *p_config, const std::string &db_name);

 private:
  /// Login with db
  int Login(const string &db);

 public:
  LinkConfig *p_config_;
  std::string db_name_;

  mongo::DBClientConnection conn_;    ///< connection with mongodb
 private:
  DISALLOW_COPY_AND_ASSIGN(MongoBase);
};

class MongoBody: public MongoBase {
 public:
  MongoBody() {
  }

  ~MongoBody() {
  }

  /// param[in] task_id Only for static task, default to -1
  int StoreBody(const std::string &url,
                const std::string &seed_url,
                const std::string &referer,
                int depth,
                const std::string &body,
                int task_id = -1);
};

class MongoExtract: public MongoBase {
 public:
  MongoExtract() {
  }

  ~MongoExtract() {
  }

  /// param[in] task_id Only for static task, default to -1
  int StoreExtract(const std::string &url,
                   const std::string &seed_url,
                   const std::string &referer,
                   int depth,
                   const MatchedResultItem &matched_result_item,
                   int task_id = -1);
};

class MongoSeed: public MongoBase {
 public:
  MongoSeed() {
  }

  ~MongoSeed() {
  }

  int QueryStaticSeeds(int task_id, std::list<StaticSeedItem> *p_seed); 
};


}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_MONGO_STORAGE_H_
