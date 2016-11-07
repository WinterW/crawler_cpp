/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/redis/redis_config.cc
 * @namespace ganji::crawler::conf_crawler::dedup::redis
 * @version 1.0
 * @author  zdn
 * @date    2014-02-17
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/util/cache/redis/redis_config.h"

#include <stdlib.h>

#include "ganji/util/config/config.h"

using std::string;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup { namespace redis {
using ganji::util::config::Config;

int RedisConfig::CheckVal(int val, const string &name) {
  if (val <= 0) {
    fprintf(stderr, "%s:%d <= 0\n", name.c_str(), val);
    exit(1);
  }
  return 0;
}

int RedisConfig::LoadConfig(const string &conf_file) {
  Config conf;
  conf.loadConfFile(conf_file);
  conf.getItemValue(kUseRedis, use_redis_);
  conf.getItemValue(kRedisHost, redis_ip_);
  conf.getItemValue(kRedisPort, redis_port_);

  CheckVal(redis_port_, "redis_port_");
  return 0;
}

void RedisConfig::PrintConfig() const {
  fprintf(stdout, "--------------------------Redis config--------------------------\n");

  fprintf(stdout, "redis host:%s\n", redis_ip_.c_str());
  fprintf(stdout, "redis port:%d\n", redis_port_);
  fprintf(stdout, "--------------------------Redis config--------------------------\n");
}
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup::redis

