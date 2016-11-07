
/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/util/cache/redis/gtest/pressure_test.cc
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  zdn
 * @date    2014-02-20
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <boost/regex.hpp>
#include "ganji/util/cache/redis/gtest/pressure_test.h"
#include "ganji/util/cache/redis/redis.h"
#include "ganji/util/log/thread_fast_log.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/system/system.h"
#include "ganji/util/time/time.h"
#include "gtest/gtest.h"

using std::string;

using boost::smatch;
using boost::regex;
using boost::regex_search;
using boost::regex_error;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

using ganji::util::thread::Thread;
using namespace ganji::util::cache::redis;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace System = ::ganji::util::system::System;
namespace Time = ::ganji::util::time;
namespace redis = ::ganji::util::cache::redis;
namespace ganji { namespace util { namespace cache {namespace redis { namespace gtest {
PressureTest::PressureTest()
{

}

PressureTest::~PressureTest()
{
  if (redis_client_ != NULL)
    delete redis_client_;
}

int PressureTest::BatchRemove(const std::string &url) {
  hashkey_lock_.RdLock();
  if ( REDIS_SUCCESS != redis_client_->RedisKeyDEL(current_hash_key_))
  {
      hashkey_lock_.Unlock();
      return -1;
  }
  hashkey_lock_.Unlock();
  return 0;
}

void PressureTest::ComposeHashKey(string prefix, string suffix)
{
  // current hash key is dynYYYYMMDD for static dedup module
  WriteLog(kLogDebug, "%s", current_hash_key_.c_str());
  current_hash_key_ = prefix;
  current_hash_key_.append(suffix);
}


int PressureTest::Init() {
  redis_client_ = new Redis;
  if (redis_client_ == NULL || redis_client_->Init() != redis::REDIS_SUCCESS)
     return -1;
  string today;
  Time::GetY4MD2(time(NULL),&today);
  ComposeHashKey("stat", today);
  string batchkey("stat*"); //flash db for static dedup
  BatchRemove(batchkey);
  return 0;
}
TEST_T(PressureTest, getset)
{

}

}}}}}
