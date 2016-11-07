/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/util/cache/redis_config.h
 * @namespace ganji::util::cache::redis
 * @version 1.0
 * @author  zdn
 * @date    2014-02-17
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef __GANJI_UTIL_CACHE_REDISCONFIG_
#define __GANJI_UTIL_CACHE_REDISCONFIG_

#include <string>

#include "global.h"

namespace ganji { namespace util { namespace cache { namespace redis {
using namespace std;
const char kUseRedis[] = "USE_REDIS";
const char kRedisHost[] = "REDIS_HOST";
const char kRedisPort[] = "REDIS_PORT";
const char kNbThreadCount[] = "NB_THREAD_COUNT";
const char kBloomFilterMaxElem[] = "BF_MAX_ELEM";
const char kBloomFilterFPF[] = "BF_FPF";
const char kBucketCount[] = "BUCKET_COUNT";
const char kDayCount[] = "DAY_COUNT";
const char kMd5Path[] = "MD5_PATH";

class RedisConfig {
 public:
  RedisConfig() { }

  int LoadConfig(const std::string &conf_file);

  void PrintConfig() const;

  int UseRedis() { return use_redis_;}
  int GetRedisPort() { return redis_port_; }
  string GetRedisIP() { return redis_ip_; }

 private:
  int CheckVal(int val, const std::string &name);

 private:
  int use_redis_;
  int redis_port_;
  string redis_ip_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RedisConfig);
};
}}}};  ///< end of namespace ganji::util::cache::redis

#endif  ///< __GANJI_UTIL_CACHE_REDISCONFIG_
