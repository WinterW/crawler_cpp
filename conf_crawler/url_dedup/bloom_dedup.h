/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/bloom_dedup.h
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_BLOOM_DEDUP_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_BLOOM_DEDUP_H_

#include <string>

#include "ganji/ganji_global.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"

class bloom_filter;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
class DedupConfig;

using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;

/**
 * @class BloomDedup
 * @brief url deduplication
 */
class BloomDedup {
 public:
  BloomDedup():
    p_config_(NULL),
    p_bloom_filter_(NULL) {
  }

  ~BloomDedup();

  int Init(DedupConfig *p_config);

  /// @brief url在bloom filter中是否存在
  bool IsExists(const std::string &url);

  /// @brief 插入url
  void Insert(const std::string &url);

  /// @brief 检查url在bloom filter中是否存在，如果不存在，则insert
  /// @param[in] url 待检查的url
  /// @param[out] p_exists url在bloom filter中是否存在
  void TestExistsAndInsert(const std::string &url, bool *p_exists);

 private:

 private:
  DedupConfig *p_config_;

  bloom_filter *p_bloom_filter_;          ///< bloom filter
  Mutex bf_lock_;

 private:
  DISALLOW_COPY_AND_ASSIGN(BloomDedup);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_BLOOM_DEDUP_H_
