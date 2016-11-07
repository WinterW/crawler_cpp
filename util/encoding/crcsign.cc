/**
 * @Copyright (c) 2011 Ganji Inc.
 * @namespace ganji::util::encoding
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-09-08
 *
 */

#include "crcsign.h"
#include <stdio.h>

namespace ganji { namespace util { namespace encoding {

uint CrcSign::crc32_table_[256];
ulong CrcSign::crc64_table_[256];

void CrcSign::Init() {
  Init32();
  Init64();
}

void CrcSign::Sign(char const str[], uint& sign) {
  sign = INIT32CRC;
  if (str == NULL) return;
  while(*str != '\0') {
    sign = crc32_table_[(sign ^ *str++) & 0xff] ^ (sign >> 8);
  }
}

void CrcSign::Sign(char const str[], ulong& sign) {
  sign = INIT64CRC;
  if (str == NULL) return;
  while(*str != '\0') {
    sign = crc64_table_[(sign ^ *str++) & 0xff] ^ (sign >> 8);
  }
}

void CrcSign::Sign(char const str[], char* signstr){
  ulong sign = INIT64CRC;
  if (str == NULL) return;
  while(*str != '\0') {
    sign = crc64_table_[(sign ^ *str++) & 0xff] ^ (sign >> 8);
  }
  snprintf(signstr, 17, "%lx", sign);
}

void CrcSign::Init32() {
  if (crc32_table_[0] != 0) return;
  int part;
  int i, j;
  for (i = 0; i < 256; ++i) {
    part = i;
    for (j = 0; j < 8; ++j) {
      if (part & 1){
        part = (part >> 1) ^ POLY32REV;
      } else {
        part >>= 1;
      }
    }
    crc32_table_[i] = part;
  }
}

void CrcSign::Init64() {
  if (crc64_table_[0] != 0) return;
  ulong part;
  int i, j;
  for (i = 0; i < 256; ++i) {
    part = i;
    for (j = 0; j < 8; ++j) {
      if (part & 1) {
        part = (part >> 1) ^ POLY64REV;
      } else {
        part >>= 1;
      }
    }
    crc64_table_[i] = part;
  }
}
} } }
