/**
 * @Copyright (c) 2011 Ganji Inc.
 * @file    ganji/util/encoding/encryption.h
 * @namespace ganji::util::encoding
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-09-08
 *
 */

#ifndef _GANJI_UTIL_ENCODING_ENCRYPTION_H_
#define _GANJI_UTIL_ENCODING_ENCRYPTION_H_

namespace ganji { namespace util { namespace encoding {

class Encryption {
public:
  // out最大占用长度为  str长度*2+15
  static int XorEncode(const char str[], const long key, char out[]);

  static int XorDecode(const char str[], const long key, char out[]);
private:
  static const int endian_;
};

} } }

#endif ///< _GANJI_UTIL_ENCODING_ENCRYPTION_H_
