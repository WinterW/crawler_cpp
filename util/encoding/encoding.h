/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/encoding.h
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 * string encoding options!
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_ENCODING_ENCODING_H_
#define _GANJI_UTIL_ENCODING_ENCODING_H_


#include "global.h"

namespace ganji { namespace util { namespace encoding { namespace Encoding {

/// 十进制数字转换为十六进制字符:0-15转换为'0'-'F'
/// @param[in] n The decimal number
/// @return The hex char
char Dec2HexChar(int16_t n);

/// 十六进制字符转换成十进制的数字:'0'-'F'或'0'-'f'转换为0-15
/// @param[in] c The hex char to be converted
/// @return The decimal number
int16_t HexChar2Dec(char c);

} } } }   ///< end of namespace ganji::util::encoding::Encoding
#endif  ///< _GANJI_UTIL_ENCODING_ENCODING_H_
