/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_config.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-24
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_CONFIG_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_CONFIG_H_

#include <string>

#include "global.h"
#include "util/config/config.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
using std::string;
using ganji::util::config::Config;

const char kNbThreadCount[] = "NB_THREAD_COUNT";
const char kSocketTimeout[] = "SOCKET_TIMEOUT";
const char kPersistCount[] = "PERSIST_COUNT";
const char kCheckInterval[] = "CHECK_INTERVAL";
const char kSaveDbInterval[] = "SAVE_DB_INTERVAL";
const char kLinkBasePort[] = "LINK_BASE_PORT";
const char kDcHost[] = "DC_HOST";
const char kDcPort[] = "DC_PORT";
const char kExtractorHost[] = "EXTRACTOR_HOST";
const char kExtractorPort[] = "EXTRACTOR_PORT";
const char kDedupHost[] = "URL_DEDUP_HOST";
const char kDedupPort[] = "URL_DEDUP_PORT";
const char kDbHost[] = "DB_HOST";
const char kDbPort[] = "DB_PORT";
const char kDbUser[] = "DB_USER";
const char kDbPasswd[] = "DB_PASSWD";
const char kDbDatabase[] = "DB_DATABASE";
const char kMongoHostPort[] = "MONGO_HOST_PORT";
const char kMongoBodyDb[] = "MONGO_BODY_DB";
const char kMongoExtractDb[] = "MONGO_EXTRACT_DB";
const char kMongoImgDb[] = "MONGO_IMG_DB";
const char kMongoSeedDb[] = "MONGO_SEED_DB";
const char kMongoUser[] = "MONGO_USER";
const char kMongoPasswd[] = "MONGO_PASSWD";
const char kRetryTimes[] = "RETRY_TIMES";
const char kNightStart[] = "NIGHT_START";
const char kNightEnd[] = "NIGHT_END";
const char kFreqInit[] = "FREQ_INIT";
const char kFreqDullRate[] = "FREQ_DULL_RATE";
const char kFreqDullDullRate[] = "FREQ_DULL_DULL_RATE";
const char kFreqNightContribRate[] = "FREQ_NIGHT_CONTRIB_RATE";
const char kFreqIncrRate[] = "FREQ_INCR_RATE";
const char kFreqDecrRate[] = "FREQ_DECR_RATE";

class LinkConfig {
 public:
  LinkConfig() { }

  int LoadConfig(const string &conf_file);

  void PrintConfig() const;

  int GetNbThreadCount() { return nb_thread_count_; }
  int GetSocketTimeout() { return socket_timeout_; }
  int GetPersistCount() { return persist_count_; }
  int GetCheckInterval() { return check_interval_; }
  int SaveDbInterval() { return save_db_interval_; }
  int GetLinkBasePort() { return link_base_port_; }
  const string & GetDcHost() { return dc_host_; }
  int GetDcPort() { return dc_port_; }
  const string & GetExtractorHost() { return extractor_host_; }
  int GetExtractorPort() { return extractor_port_; }
  const string & GetDedupHost() { return dedup_host_; }
  int GetDedupPort() { return dedup_port_; }
  const string &GetDbHost() { return db_host_; }
  const string &GetDbPort() { return db_port_; }
  const string &GetDbUser() { return db_user_; }
  const string &GetDbPasswd() { return db_passwd_; }
  const string &GetDbDatabase() { return db_database_; }
  const string &GetMongoHostPort() { return mongo_host_port_; }
  const string &MongoBodyDb() { return mongo_body_db_; }
  const string &MongoExtractDb() { return mongo_extract_db_; }
  const string &MongoImgDb() { return mongo_img_db_; }
  const string &GetMongoSeedDb() { return mongo_seed_db_; }
  const string &GetMongoUser() { return mongo_user_; }
  const string &GetMongoPasswd() { return mongo_passwd_; }
  int GetRetryTimes() { return retry_times_; }
  int FreqInit() { return freq_init_; }
  float FreqDullRate() { return freq_dull_rate_; }
  float FreqDullDullRate() { return freq_dull_dull_rate_; }
  float FreqNightContribRate() { return freq_night_contrib_rate_; }
  float FreqIncrRate() { return freq_incr_rate_; }
  float FreqDecrRate() { return freq_decr_rate_; }
  const string &NightStart() { return night_start_; }
  const string &NightEnd() { return night_end_; }

 private:
  int CheckVal(int val, const string &name);
  void GetFloatVal(const string &name, Config *p_conf, float *p_val);

 private:
  int nb_thread_count_;
  int socket_timeout_;
  int persist_count_;
  int check_interval_;
  int save_db_interval_;
  int link_base_port_;
  string dc_host_;
  int dc_port_;
  string extractor_host_;
  int extractor_port_;
  string dedup_host_;
  int dedup_port_;
  string db_host_;
  string db_port_;
  string db_user_;
  string db_passwd_;
  string db_database_;
  string mongo_host_port_;
  string mongo_body_db_;
  string mongo_extract_db_;
  string mongo_img_db_;
  string mongo_seed_db_;
  string mongo_user_;
  string mongo_passwd_;
  int retry_times_;             ///< retry times for request to deduplicate svr

  string night_start_;          ///< start time for night, HHMM
  string night_end_;            ///< end time for night, HHMM

  int freq_init_;               ///< initial value for frequency when it is from night into day
  float freq_dull_rate_;        ///< frequency rate if #post of previous interval == 0
  float freq_dull_dull_rate_;   ///< frequency rate if #post of previous && previous to previous interval == 0
  float freq_night_contrib_rate_; ///< frequency contribute rate when it is night
  float freq_incr_rate_;        ///< abs incremental rate
  float freq_decr_rate_;        ///< abs decremental rate

 private:
  DISALLOW_COPY_AND_ASSIGN(LinkConfig);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_CONFIG_H_
