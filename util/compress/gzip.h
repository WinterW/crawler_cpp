/**
 * @Copyright 2011 Ganji Inc.
 * @file    ganji/util/compress/gzip.h
 * @namespace ganji::util::compress
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-20
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_UTIL_COMPRESS_GZIP_H_
#define _GANJI_UTIL_COMPRESS_GZIP_H_

#include <string>

namespace ganji { namespace util { namespace compress { namespace Gzip {
const int kChunk = 16384;

/// deflate src
/// @param[in] src The source string to deflate
/// @param[out] p_def The deflated string
/// @param[in/out] p_def_len The lenght of the deflated string, a `value-result' argument
/// @return 0:success -1:failure
int Deflate(const std::string &src, char *p_def, size_t *p_def_len);

/// inflate src
/// @param[in] p_src The source string to inflate
/// @param[in] src_len The lenght of the string to inflate
/// @param[out] p_inf_str The inflated string
/// @return 0:success -1:failure
int Inflate(const char *p_src, size_t src_len, std::string *p_inf_str);
}
} } }   ///< end of namespace ganji::util::compress::Gzip
#endif  ///< _GANJI_UTIL_COMPRESS_GZIP_H_
