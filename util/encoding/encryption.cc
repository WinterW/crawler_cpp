/**
 * @Copyright (c) 2011 Ganji Inc.
 * @namespace ganji::util::encoding
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-09-08
 *
 */

#include <string.h>
#include <stdio.h>
#include "encryption.h"

namespace ganji { namespace util { namespace encoding {
const int Encryption::endian_ = 1;

int Encryption::XorEncode(const char str[], const long key, char out[]) {
  if (str == NULL) return -1;
  int len = strlen(str) + 1;
  int i = 0;
  int j = 0;
  long seg = 0;
  while (i < len){
    seg += ((long)str[i] << ((i%8) << 3));
    ++i;
    if (i % 8 == 0 || i == len){
      j += sprintf(out + j, "%016lx", key ^ seg);
      seg = 0;
    }
  }
  return 0;
}

int Encryption::XorDecode(const char str[], const long key, char out[]){
  if (str == NULL) return -1;
  int len = strlen(str);
  long* outlong = (long*)out;
  int i = 0;
  int j = 0;
  long seg = 0;
  while (i < len){
    char a = str[i];
    if (a >= '0' && a <= '9'){
      seg = (seg << 4) + a - '0';
    } else if (a >= 'a' && a <= 'f'){
      seg = (seg << 4) + a - 87;   // 87 = 'a' - 10
    } else {
      return -1;
    }
    ++i;
    if (i % 16 == 0 || i == len){
      if  (*(char*)&endian_ == 1){
        outlong[j++] = (seg ^ key);
      } else {    // 大端序
        long tmplong = (seg ^ key);
        char* temp = (char*)&tmplong;
        long bendian = 0;
        for (int k = 7; k >= 0; --k){
          bendian += (temp[7-k] << (k << 3));
        }
        outlong[j++] = bendian;
      }
      seg = 0;
    }
  }
  return 0;
}

} } }
