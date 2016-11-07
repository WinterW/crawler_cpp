/**
 * @Copyright (c) 2011 Ganji Inc.
 * @file    ganji/util/encoding/crcsign.h
 * @namespace ganji::util::encoding
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-09-08
 *
 */

#ifndef _GANJI_UTIL_ENCODING_CRCSIGN_H_
#define _GANJI_UTIL_ENCODING_CRCSIGN_H_

#include <sys/types.h>
#include <stdlib.h>

namespace ganji { namespace util { namespace encoding {

#define POLY64REV 0x95AC9329AC4BC9B5ULL
#define INIT64CRC 0xFFFFFFFFFFFFFFFFULL

#define POLY32REV 0x04C11DB7UL
#define INIT32CRC 0xFFFFFFFFUL

class CrcSign {
public:
  static void Init();

  static void Sign(char const str[], uint& sign);

  static void Sign(char const str[], ulong& sign);

  // sign分配的长度必须大于17
  static void Sign(char const str[], char* sign);
private:
  static void Init32();

  static void Init64();
private:
  static uint crc32_table_[256];
  static ulong crc64_table_[256];
};
} } }
#endif ///< _GANJI_UTIL_ENCODING_CRCSIGN_H_
