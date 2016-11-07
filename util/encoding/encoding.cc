/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/encoding.cc
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */


#include "encoding.h"

namespace ganji { namespace util { namespace encoding { namespace Encoding {

char Dec2HexChar(int16_t n) {
  if ( 0 <= n && n <= 9 ) {
    return static_cast<char>(int16_t('0') + n);
  } else if ( 10 <= n && n <= 15 ) {
    return static_cast<char>(int16_t('A') + n - 10);
  } else {
    return 0;
  }
}

int16_t HexChar2Dec(char c) {
  if ('0' <= c && c <= '9') {
    return int16_t(c -'0');
  } else if ('a' <= c && c <= 'f') {
    return (int16_t(c -'a') + 10);
  } else if ('A' <= c && c <= 'F') {
    return (int16_t(c -'A') + 10);
  } else {
    return -1;
  }
}
} } } }   ///< end of namespace ganji::util::encoding::Encoding
