/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/utf8conv.h
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 * utf8 convert options.
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_ENCODING_UTF8CONV_H_
#define _GANJI_UTIL_ENCODING_UTF8CONV_H_


#include<string>
#include<vector>
#include "global.h"

namespace ganji { namespace util { namespace encoding { namespace Utf8Conv {

/// 把utf8源字符串转换成gbk, 注意，不会检查源字符串是否是合法的utf8串
/// @param[in] str_utf8 utf8 string
/// @return gbk string
std::string Utf8ToGBK(const std::string &str_utf8);

/// 把gbk源字符串转换成utf8, 注意，不会检查源字符串是否是合法的gbk串
/// @param[in] gbk string
/// @return utf8 string
std::string GBKToUtf8(const std::string &str_locale);

/// 把utf字符串逐字符载入到一个vector里,并记录字符起始位置在原始串中的位置
///     不会检查源字符串是否是合法的utf8串
/// @param[in] str, 原始字符串
/// @param[out] pvec, 结束后， vec里每个元素都是一个完整的utf8字符。
void Utf8ToVector(const std::string &str , std::vector<std::pair<std::string, size_t> > *pvec);

} } } }   ///< end of namespace ganji::util::encoding::Utf8Conv
#endif  ///< _GANJI_UTIL_ENCODING_UTF8CONV_H_ 
