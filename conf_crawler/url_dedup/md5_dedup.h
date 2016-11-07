/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/md5_dedup.h
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-23
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_MD5_DEDUP_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_MD5_DEDUP_H_

#include <string>

#include "ganji/ganji_global.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"
#include "ganji/util/thread/rwlock.h"
#include "ganji/util/thread/thread.h"

#include "ganji/crawler/conf_crawler/url_dedup/struct_def.h"
#include "ganji/crawler/conf_crawler/url_dedup/conf_crawler_types.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
class DedupConfig;

using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::util::thread::Thread;

/**
 * @class Md5Dedup
 * @brief url deduplication
 */
class Md5Dedup {
 public:
  Md5Dedup():
    p_config_(NULL),
    p_set_list_(NULL) {
  }

  ~Md5Dedup();

  int Init(DedupConfig *p_config);

  void IsExists(const std::string &url, DedupExistItem *p_exist_item);

  /// @brief 插入url
  bool Insert(const std::string &url);

  /// @brief 检查url在bloom filter中是否存在，如果不存在，则insert
  /// @param[in] url 待检查的url
  /// @param[out] p_exists url在bloom filter中是否存在
  bool TestExistsAndInsert(const std::string &url, bool *p_exists);

  /// @brief remove one url
  /// @param[in] url the url to remove
  /// @return true:url found
  bool Remove(const std::string &url);

  /// Get the internal information
  /// @param[out] p_info The internal info
  void Info(std::string *p_info);

 private:
  /// allocate set for today
  void CreateTodaySet();

  /// Load md5 of past days from md5 path
  void LoadSavedMd5();

  /// get url md5
  /// @param[in] url The url
  /// @param[out] p_url_md5 The generated url md5
  /// @return 0:success -1:failure
  int GetUrlMd5(const std::string &url, UrlMd5 *p_url_md5);

  /// Switch day
  int SwitchDay();

  /// Switch hour
  int SwitchHour();

  /// Save md5 of yesterday
  /// @return 0:success -1:failure
  int SaveYesterdayMd5();

  /// Save md5 into file
  /// @param[in] p_md5_set The Md5Set to save
  /// @param[in] output_file The file to save into
  /// @return 0:success -1:failure
  int SaveMd5File(UrlMd5Set *p_md5_set, const std::string &output_file);

 private:
  static void *TimerThread(void *arg);

 private:
  DedupConfig *p_config_;
  int day_count_;

  UrlMd5SetPtr *p_set_list_;         ///< pointer to each set for each day, with elem[0] for today
  RWLock url_lock_;

  Thread *p_timer_thread_;

  std::string hour_;                  ///< HH, for switching hour
  std::string today_;                 ///< YYYYMMDD, for switching day
 private:
  DISALLOW_COPY_AND_ASSIGN(Md5Dedup);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STATIC_DEDUP_H_
