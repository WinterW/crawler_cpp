/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_config.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-24
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/link_base/link_config.h"

#include <stdlib.h>

#include "util/config/config.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
using ganji::util::config::Config;

int LinkConfig::CheckVal(int val, const string &name) {
  if (val <= 0) {
    fprintf(stderr, "%s:%d <= 0\n", name.c_str(), val);
    exit(1);
  }
  return 0;
}

int LinkConfig::LoadConfig(const string &conf_file) {
  Config conf;
  conf.loadConfFile(conf_file);
  conf.getItemValue(kNbThreadCount, nb_thread_count_);
  conf.getItemValue(kSocketTimeout, socket_timeout_);
  conf.getItemValue(kPersistCount, persist_count_);
  conf.getItemValue(kCheckInterval, check_interval_);
  conf.getItemValue(kSaveDbInterval, save_db_interval_);
  conf.getItemValue(kLinkBasePort, link_base_port_);
  conf.getItemValue(kDcHost, dc_host_);
  conf.getItemValue(kDcPort, dc_port_);
  conf.getItemValue(kExtractorHost, extractor_host_);
  conf.getItemValue(kExtractorPort, extractor_port_);
  conf.getItemValue(kDedupHost, dedup_host_);
  conf.getItemValue(kDedupPort, dedup_port_);
  conf.getItemValue(kDbHost, db_host_);
  conf.getItemValue(kDbPort, db_port_);
  conf.getItemValue(kDbUser, db_user_);
  conf.getItemValue(kDbPasswd, db_passwd_);
  conf.getItemValue(kDbDatabase, db_database_);
  conf.getItemValue(kMongoHostPort, mongo_host_port_);
  conf.getItemValue(kMongoBodyDb, mongo_body_db_);
  conf.getItemValue(kMongoExtractDb, mongo_extract_db_);
  conf.getItemValue(kMongoImgDb, mongo_img_db_);
  conf.getItemValue(kMongoSeedDb, mongo_seed_db_);
  conf.getItemValue(kMongoUser, mongo_user_);
  conf.getItemValue(kMongoPasswd, mongo_passwd_);
  conf.getItemValue(kRetryTimes, retry_times_);

  conf.getItemValue(kNightStart, night_start_);
  conf.getItemValue(kNightEnd, night_end_);

  conf.getItemValue(kFreqInit, freq_init_);
  GetFloatVal(kFreqDullRate, &conf, &freq_dull_rate_);
  GetFloatVal(kFreqDullDullRate, &conf, &freq_dull_dull_rate_);
  GetFloatVal(kFreqNightContribRate, &conf, &freq_night_contrib_rate_);
  GetFloatVal(kFreqIncrRate, &conf, &freq_incr_rate_);
  GetFloatVal(kFreqDecrRate, &conf, &freq_decr_rate_);

  CheckVal(nb_thread_count_, "nb_thread_count_");
  CheckVal(socket_timeout_, "socket_timeout_");
  CheckVal(persist_count_, "persist_count_");
  CheckVal(check_interval_, "check_interval_");
  CheckVal(link_base_port_, "link_base_port_");
  CheckVal(dc_port_, "dc_port_");
  CheckVal(extractor_port_, "extractor_port_");
  CheckVal(dedup_port_, "dedup_port_");
  CheckVal(retry_times_, "retry_times_");

  return 0;
}

void LinkConfig::GetFloatVal(const string &name, Config *p_conf, float *p_val) {
  string value;
  p_conf->getItemValue(name, value);
  *p_val = atof(value.c_str());
}

void LinkConfig::PrintConfig() const {
  fprintf(stdout, "--------------------------LinkConfig config--------------------------\n");

  fprintf(stdout, "nb thread count:%d\n", nb_thread_count_);
  fprintf(stdout, "socket timeout:%d\n", socket_timeout_);
  fprintf(stdout, "persist count:%d\n", persist_count_);
  fprintf(stdout, "check interval:%d\n", check_interval_);
  fprintf(stdout, "save db interval:%d\n", save_db_interval_);
  fprintf(stdout, "link base port:%d\n", link_base_port_);
  fprintf(stdout, "dc host:%s\n", dc_host_.c_str());
  fprintf(stdout, "dc port:%d\n", dc_port_);
  fprintf(stdout, "extractor host:%s\n", extractor_host_.c_str());
  fprintf(stdout, "extractor port:%d\n", extractor_port_);
  fprintf(stdout, "url dedup host:%s\n", dedup_host_.c_str());
  fprintf(stdout, "url dedup port:%d\n", dedup_port_);
  fprintf(stdout, "db host:%s\n", db_host_.c_str());
  fprintf(stdout, "db port:%s\n", db_port_.c_str());
  fprintf(stdout, "db user:%s\n", db_user_.c_str());
  fprintf(stdout, "db passwd:%s\n", db_passwd_.c_str());
  fprintf(stdout, "db database:%s\n", db_database_.c_str());
  fprintf(stdout, "mongo host port:%s\n", mongo_host_port_.c_str());
  fprintf(stdout, "mongo_body_db_:%s\n", mongo_body_db_.c_str());
  fprintf(stdout, "mongo_extract_db_:%s\n", mongo_extract_db_.c_str());
  fprintf(stdout, "mongo_img_db_:%s\n", mongo_img_db_.c_str());
  fprintf(stdout, "mongo seed db:%s\n", mongo_seed_db_.c_str());
  fprintf(stdout, "mongo user:%s\n", mongo_user_.c_str());
  fprintf(stdout, "mongo passwd:%s\n", mongo_passwd_.c_str());
  fprintf(stdout, "retry times:%d\n", retry_times_);

  fprintf(stdout, "night_start_:%s\n", night_start_.c_str());
  fprintf(stdout, "night_end_:%s\n", night_end_.c_str());

  fprintf(stdout, "freq_init_:%d\n", freq_init_);
  fprintf(stdout, "freq_dull_rate_:%f\n", freq_dull_rate_);
  fprintf(stdout, "freq_dull_dull_rate_:%f\n", freq_dull_dull_rate_);
  fprintf(stdout, "freq_night_contrib_rate_:%f\n", freq_night_contrib_rate_);
  fprintf(stdout, "freq_incr_rate_:%f\n", freq_incr_rate_);
  fprintf(stdout, "freq_decr_rate_:%f\n", freq_decr_rate_);

  fprintf(stdout, "--------------------------LinkConfig config--------------------------\n");
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dc

