/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/util/cache/redis/redis.h
 * @namespace ganji::util::cache::redis
 * @version 1.0
 * @author  zdn
 * @date    2014-02-17
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef __GANJI_UTIL_CACHE_REDIS_
#define __GANJI_UTIL_CACHE_REDIS_

#include <string>
#include <vector>


#include "global.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"
#include "util/thread/thread.h"
#include "hiredis.h"

namespace ganji { namespace util { namespace cache { namespace redis {

class RedisConfig;
using namespace std;
using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::util::thread::Thread;

#define CMD_MAX_LEN 1024
#define VAL_LEN 1024

enum {
  REDIS_SUCCESS = 0,
  REDIS_KEY_EXIST = 1,
  REDIS_ERROR   = 2,
  REDIS_FAIL,
  REDIS_COMMAND_FAIL,
  REDIS_CONNECTION_ERROR
};

/**
 * @class Redis
 * @thread safe redis client
 */
class Redis {
 public:
  Redis();
  ~Redis();

  int Init();

  /// @brief key在redis中是否存在
  virtual bool IsExists(const std::string &key);

  /// @brief 插入key
  virtual bool Insert(const std::string &key);

  /// @param[in] key 待检查的key
  /// @param[out] p_exists key在redis中是否存在
  virtual bool TestExistsAndInsert(const std::string &key, bool *p_exists);

  /// @brief rehash hash table
  /// @param[in] bucket_count new bucket count
  virtual void Rehash(int bucket_count);

  /// @brief remove one key
  /// @param[in] key the key to remove
  /// @return true:key found
  virtual bool Remove(const std::string &key);

  /// @brief batch remove by key pattern
  /// @param[in] key_pattern the key pattern to remove
  /// @return number of key to remove
  virtual int BatchRemove(const std::string &key_pattern);

  int RediskeyEXISTS(const string& key);
  int RediskeyEXPIRE(const string& key, int seconds);
  int RedisGET(const string& key, string& value);
  int RedisSETNX(const string& key, string& value);
  int RedisSETEX(const string& key, int seconds, string& value);
  int RedisKeyDEL(const string& key);
  int RedisHSET(const string& hash, const string& key, string& value);
  int RedisHEXISTS(const string& hash, const string& key);
  int RedisHSETNX(const string& hash, const string& key, string& value);
  int RedisHDEL(const string& hash, const string& key);
  int RedisHGET(const string& hash, const string& key, string& value);
  int RedisDBSize(string& dbsize);
 protected:
  int GetConnectionState();
  int Connect();
  int RedisSingleCmd(redisContext* redis_context, const char* cmd, string& reply_code);
  int RedisMutipleCmd(redisContext* redis_context, const vector<string>& vec_cmd);
  int RedisCmdWithMutipleReply(redisContext* redis_context, const char* vec_cmd, vector<string>& vec_reply);
  int RedisTransaction(redisContext* redis_context, const vector<string>& vec_cmd, vector<string>& vec_reply);
  static void *TimerThread(void *arg);

  RedisConfig *p_config_;
  string redis_ip_;
  int redis_port_;
  redisContext* redis_context_;
  RWLock db_lock_;
  Thread *p_timer_thread_;
 private:
  DISALLOW_COPY_AND_ASSIGN(Redis);
};
}}}};  ///< end of namespace ganji::util::cache::redis

#endif  ///< __GANJI_UTIL_CACHE_REDIS_
