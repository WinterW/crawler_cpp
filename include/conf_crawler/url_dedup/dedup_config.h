/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/dedup_config.h
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_DEDUP_CONFIG_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_DEDUP_CONFIG_H_

#include <string>

#include "global.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
const char kDedupHost[] = "DEDUP_HOST";
const char kDedupPort[] = "DEDUP_PORT";
const char kNbThreadCount[] = "NB_THREAD_COUNT";
const char kBloomFilterMaxElem[] = "BF_MAX_ELEM";
const char kBloomFilterFPF[] = "BF_FPF";
const char kBucketCount[] = "BUCKET_COUNT";
const char kDayCount[] = "DAY_COUNT";
const char kMd5Path[] = "MD5_PATH";

class DedupConfig {
 public:
  DedupConfig() { }

  int LoadConfig(const std::string &conf_file);

  void PrintConfig() const;

  const std::string & GetDedupHost() { return dedup_host_; }
  int GetDedupPort() { return dedup_port_; }
  int GetNbThreadCount() { return nb_thread_count_; }
  int GetBloomFilterMaxElem() { return bf_max_elem_; }
  float GetBloomFilterFPF() { return bf_fpf_; }
  int BucketCount() { return bucket_count_; }
  int DayCount() { return day_count_; }
  const std::string &Md5Path() { return md5_path_; }

 private:
  int CheckVal(int val, const std::string &name);

 private:
  std::string dedup_host_;
  int dedup_port_;
  int nb_thread_count_;
  int bf_max_elem_;                 ///< bloom filter存放的最大元素数
  float bf_fpf_;                      ///< bloom filter的false positive probability
  int bucket_count_;                ///< #bucket for hash table
  int day_count_;                   ///< #day for hash table
  std::string md5_path_;            ///< path to save url md5

 private:
  DISALLOW_COPY_AND_ASSIGN(DedupConfig);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_DEDUP_CONFIG_H_
