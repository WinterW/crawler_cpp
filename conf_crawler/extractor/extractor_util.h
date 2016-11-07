/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/extractor/extractor_util.h
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-10
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_EXTRACTOR_UTIL_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_EXTRACTOR_UTIL_H_

#include <string>

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor { namespace ExtractorUtil {
void RemoveComment(const std::string &orig_body, std::string *p_body);

void ReplacePairStr(const std::string &orig_str,
                    const std::string &start_str,
                    const std::string &end_str,
                    std::string *p_str);

/// to plain text
void ToPlainText(const std::string &orig_body, std::string *p_body);
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor::ExtractorUtil

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_EXTRACTOR_UTIL_H_
