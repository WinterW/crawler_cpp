/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/util/cache/redis/gtest/pressure_test.h
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  zdn
 * @date    2014-02-20
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef __GANJI_UTIL_CACHE_REDIS_GTEST_PRESSURE_TEST_H_
#define __GANJI_UTIL_CACHE_REDIS_GTEST_PRESSURE_TEST_H_

#include <string>
#include "gtest/gtest.h"
#include "ganji/ganji_global.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"
#include "ganji/util/thread/thread.h"
#include "ganji/util/cache/redis/redis.h"
namespace ganji { namespace util { namespace cache {namespace redis { namespace gtest {

using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::util::thread::Thread;
using namespace ganji::util::cache::redis;
class PressureTest : public testing::Test{
public:
  PressureTest();
  ~PressureTest();
 virtual void SetUp()
 {
 }
 virtual void TearDown()
 {
 }
private:
  
  int BatchRemove(const std::string &url);
  void ComposeHashKey(string prefix, string suffix);
  string current_hash_key_;
  RWLock hashkey_lock_;
  Redis* redis_client_;
  DISALLOW_COPY_AND_ASSIGN(StaticDedupRedis);
};
}}}}
#endif
