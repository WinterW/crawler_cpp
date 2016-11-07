/**
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/text/stringutil.h
 * @namespace ganji::util::text
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-07-08
 *
 */

#include <stdio.h>
#include "stringutil.h"

namespace ganji { namespace util { namespace text { namespace Text {
char* GetToken(char*& str_begin, char delimiter){
  char* p = str_begin;
  char* p_end = NULL;
  if (str_begin == 0){
    return 0;
  }

  unsigned char first_byte;
  unsigned char second_byte;
  p_end = str_begin;
  while (*p_end){
    if (*p_end == delimiter){
      break;
    }
    first_byte = *p_end;
    second_byte = *(p_end+1);

    if (first_byte >= 0x81 && first_byte <= 0xfe
      && second_byte >= 0x40 && second_byte <= 0xfe){
      ++p_end;
    }
    ++p_end;
  }
  if (0 != *p_end){
    *p_end = 0;
    str_begin = p_end + 1;
  }
  else {
    str_begin = 0;
  }
  return p;
}

void Split(const string& str, char delimiter, vector<string>& vec,
    const int maxsize){
  maxStrLen = maxsize;
  char buf[maxsize];
  snprintf(buf, maxsize, "%s", str.c_str());
  char* temp = buf;
  while (char* token = GetToken(temp, delimiter)){
    vec.push_back(token);
  }
}

void Split(const string& str, char delimiter, map<string, string>& result_map,
    const int maxsize){
  maxStrLen = maxsize;
  char buf[maxsize];
  snprintf(buf, maxsize, "%s", str.c_str());
  char* temp = buf;
  vector<string> vec;
  while (char* token = GetToken(temp, delimiter)){
    vec.push_back(token);
  }
  if(vec.size() > 1){
    result_map.insert(make_pair(vec[0],vec[1]));
  }
}
}}}}
