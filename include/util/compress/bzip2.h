/**
 * @Copyright 2011 Ganji Inc.
 * @file    ganji/util/compress/bzip2.h
 * @namespace ganji::util::compress
 * @version 1.0
 * @author  lisizhong
 * @date    2012-05-16
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_UTIL_COMPRESS_BZIP2_H_
#define _GANJI_UTIL_COMPRESS_BZIP2_H_

#include <string>

namespace ganji { namespace util { namespace compress { namespace Bzip2 {
/// Get the recommended length for compressed string
/// @param[in] src_len The length of the uncompressed string
/// @return the length of the recommended length
size_t CompressedLen(size_t src_len);

/// Compress
/// @param[in] src The source string to deflate
/// @param[out] p_def The deflated string
/// @param[in/out] p_def_len The lenght of the deflated string, a `value-result' argument
/// @return 0:success -1:failure
int Compress(const std::string &src, char *p_def, size_t *p_def_len);

/// Decompress
/// @param[in] p_src The source string to inflate
/// @param[in] src_len The lenght of the string to inflate
/// @param[out] p_inf_str The inflated string
/// @return 0:success -1:failure
int Decompress(const char *p_src, size_t src_len, std::string *p_inf_str);
}
} } }   ///< end of namespace ganji::util::compress::Bzip2
#endif  ///< _GANJI_UTIL_COMPRESS_BZIP2_H_
