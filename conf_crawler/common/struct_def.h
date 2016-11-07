/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/common/struct_def.h
 * @namespace ganji::crawler::conf_crawler
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_COMMON_STRUCT_DEF_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_COMMON_STRUCT_DEF_H_

#include <string.h>
#include <hash_fun.h>
#include <string>
#include <vector>
#include <map>

namespace ganji { namespace crawler { namespace conf_crawler { 
/// seperator for POST request with POST parameters
const char kPostParam[] = "<%param%>";

/**
 * @struct for unordered_xxx
 */
struct StringHash {
  size_t operator()(const std::string &key) const {
    return __gnu_cxx::__stl_hash_string(key.c_str());
  }
  bool operator()(const std::string &s1, const std::string &s2) const {
    return strcmp(s1.c_str(), s2.c_str()) == 0;
  }
};

typedef std::map<std::string, std::vector<std::string> > SelfResult;
typedef std::vector<SelfResult> SubResultList;
}}};  ///< end of namespace ganji::crawler::conf_crawler

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_COMMON_STRUCT_DEF_H_
