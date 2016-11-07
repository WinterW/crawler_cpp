/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/extractor/extractor_util.cc
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-10
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */


#include "conf_crawler/extractor/extractor_util.h"

#include <string.h>

using std::string;

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor { namespace ExtractorUtil {
void RemoveComment(const string &orig_body, string *p_body) {
  const char kStart[] = "<!--";
  const char kEnd[] = "-->";
  ReplacePairStr(orig_body, kStart, kEnd, p_body);
}

void ReplacePairStr(const string &orig_str,
                    const string &start_str,
                    const string &end_str,
                    string *p_str) {
  size_t prev_pos = 0;
  while (true) {
    size_t start_pos = orig_str.find(start_str, prev_pos);
    if (start_pos == string::npos)
      break;
    size_t end_pos = orig_str.find(end_str, start_pos+start_str.size());
    if (end_pos == string::npos)
      break;
    *p_str += orig_str.substr(prev_pos, start_pos-prev_pos);
    prev_pos = end_pos + end_str.size();
  }

  if (prev_pos < orig_str.size())
    *p_str += orig_str.substr(prev_pos);
}

void ToPlainText(const std::string &orig_body, std::string *p_body) {
  /// replace script
  const char kScriptStart[] = "<script ";
  const char kScriptEnd[] = "</script>";
  string body_noscript;
  ReplacePairStr(orig_body, kScriptStart, kScriptEnd, &body_noscript);

  const char kStart[] = "<";
  const char kEnd[] = ">";
  ReplacePairStr(body_noscript, kStart, kEnd, p_body);
}
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor::ExtractorUtil

