/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/static_dedup.h
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-31
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STATIC_DEDUP_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STATIC_DEDUP_H_

#include <string>

#include "global.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"
#include "util/thread/rwlock.h"
#include "util/thread/thread.h"

#include "conf_crawler/url_dedup/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
class DedupConfig;

using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::util::thread::Thread;

/**
 * @class StaticDedup
 * @brief url deduplication
 */
class StaticDedup {
 public:
  StaticDedup():
    p_config_(NULL) {
  }

  ~StaticDedup();

  int Init(DedupConfig *p_config);

  /// @brief url在bloom filter中是否存在
  bool IsExists(const std::string &url);

  /// @brief 插入url
  bool Insert(const std::string &url);

  /// @brief 检查url在bloom filter中是否存在，如果不存在，则insert
  /// @param[in] url 待检查的url
  /// @param[out] p_exists url在bloom filter中是否存在
  bool TestExistsAndInsert(const std::string &url, bool *p_exists);

  /// @brief rehash hash table
  /// @param[in] bucket_count new bucket count
  void Rehash(int bucket_count);

  /// @brief remove one url
  /// @param[in] url the url to remove
  /// @return true:url found
  bool Remove(const std::string &url);

  /// @brief batch remove by url pattern
  /// @param[in] url_pattern the url pattern to remove
  /// @return number of url to remove
  int BatchRemove(const std::string &url_pattern);

 private:
  static void *TimerThread(void *arg);

 private:
  DedupConfig *p_config_;

  StringHashSet url_set_;
  RWLock url_lock_;

  Thread *p_timer_thread_;

 private:
  DISALLOW_COPY_AND_ASSIGN(StaticDedup);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STATIC_DEDUP_H_
