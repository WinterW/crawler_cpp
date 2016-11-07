/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/bloom_dedup.cc
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/url_dedup/bloom_dedup.h"
#include "ganji/crawler/conf_crawler/url_dedup/dedup_config.h"
#include "ganji/crawler/conf_crawler/url_dedup/bloom_filter.hpp"

using std::string;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
BloomDedup::~BloomDedup() {
  if (p_bloom_filter_)
    delete p_bloom_filter_;
}

int BloomDedup::Init(DedupConfig *p_config) {
  p_config_ = p_config;

  int bf_max_elem = p_config_->GetBloomFilterMaxElem();
  float bf_fpf = p_config_->GetBloomFilterFPF();
  p_bloom_filter_ = new bloom_filter(bf_max_elem, bf_fpf, 0);

  return 0;
}

bool BloomDedup::IsExists(const string &url) {
  bf_lock_.Lock();
  bool ret = p_bloom_filter_->contains(url);
  bf_lock_.Unlock();
  return ret;
}

void BloomDedup::Insert(const string &url) {
  bf_lock_.Lock();
  p_bloom_filter_->insert(url);
  bf_lock_.Unlock();
}

void BloomDedup::TestExistsAndInsert(const string &url, bool *p_exists) {
  bf_lock_.Lock();
  do {
    *p_exists = p_bloom_filter_->contains(url);
    if (*p_exists) {
      break;
    }
    p_bloom_filter_->insert(url);
  } while (0);
  bf_lock_.Unlock();
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

