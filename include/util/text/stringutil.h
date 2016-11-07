/**
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/text/stringutil.h
 * @namespace ganji::util::text
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-07-08
 *
 */

#ifndef _GANJI_UTIL_TEXT_STRINGUTIL_H_
#define _GANJI_UTIL_TEXT_STRINGUTIL_H_

#include <vector>
#include <string>
#include <map>

using std::string;
using std::vector;
using std::map;

namespace ganji { namespace util { namespace text {
namespace Text {

static size_t maxStrLen = 512;

char* GetToken(char*& str_begin, char delimiter);

void Split(const string& str, char delimiter, vector<string>& vec, const int maxsize=maxStrLen);
void Split(const string& str, char delimiter, map<string, string>& result_map, const int maxsize=maxStrLen);

}
}}}
#endif  ///< _GANJI_UTIL_TEXT_STRINGUTIL_H_
