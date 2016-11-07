/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/struct_def.h
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-31
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STRUCT_DEF_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STRUCT_DEF_H_

#include <string>
#include <unordered_set>

#include "util/encoding/md5_generator.h"
#include "conf_crawler/common/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
typedef std::unordered_set<std::string, StringHash, StringHash> StringHashSet;

/**
 * @struct UrlMd5
 * info for md5 of url
 */
struct UrlMd5 {
  unsigned char m[MD5_DIGEST_LENGTH];
  char hour;
  char minute;
  char sec;
}__attribute__((packed));

struct UrlMd5Hash {
  size_t operator()(const UrlMd5 &m) const {
    size_t h = 0;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
      h = 5 * h + m.m[i];
    return h;
  }

  bool operator()(const UrlMd5 &m1, const UrlMd5 &m2) const {
    return memcmp(m1.m, m2.m, MD5_DIGEST_LENGTH) == 0;
  }
};

typedef std::unordered_set<UrlMd5, UrlMd5Hash, UrlMd5Hash> UrlMd5Set;
typedef UrlMd5Set *UrlMd5SetPtr;
}}}};

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_URL_DEDUP_STRUCT_DEF_H_
