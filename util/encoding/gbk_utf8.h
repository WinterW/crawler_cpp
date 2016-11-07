/** 
 * @Copyright(c)  2012 Ganji Inc.
 * @file          ganji/util/encoding/gbk_utf8.h
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        lisizhong
 * @date          2010-03-07
 * 
 * convert between gbk and utf8
 * Usage: invoke GbkUtf8Conv::Init() first,
 * the parameter for Init() is the decompressed file for `gbk_utf16.tar.bz2' in current dir
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_ENCODING_GBK_UTF8_H_
#define _GANJI_UTIL_ENCODING_GBK_UTF8_H_

#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

namespace ganji { namespace util { namespace encoding { namespace GbkUtf8Conv {
const char kPadding= 0x0;
/// @brief initialize
/// @param[in] file_name gbk_utf16 map file
/// @ret 0:success -1:failure
int Init(const std::string &file_name);

/// @brief check whether str is utf8 encoded
/// @param[in] src str to check
/// @ret true:is utf8 encoded
bool IsUtf8Str(const std::string &src);

/// @brief check whether str is gbk encoded
/// @param[in] src str to check
/// @ret true:is gbk encoded
bool IsGbkStr(const std::string &src);

/// @brief convert gbk code into utf8 code
/// @param[in] src gbk code
/// @param[out] p_dest utf8 code
/// @ret 0:convert ok -1:failure
int GbkToUtf8(const std::string &src, std::string *p_dest);

/// @brief convert utf8 code into gbk code
/// @param[in] src utf8 code
/// @param[out] p_dest gbk code
/// @ret 0:convert ok -1:failure
int Utf8ToGbk(const std::string &src, std::string *p_dest);

/// @brief convert gbk code into utf8 code
/// @param[in] src gbk code
/// @param[out] p_src_len last valid pos in src
/// @param[out] dest utf8 code
/// @param[out] p_dest_len length of dest
void GbkToUtf8(const char *src, size_t *p_src_len, char *dest, size_t *p_dest_len);

/// @brief convert utf8 code into gbk code
/// @param[in] src utf8 code
/// @param[out] p_src_len last valid pos in src
/// @param[out] dest gbk code
/// @param[out] p_dest_len length of dest
void Utf8ToGbk(const char *src, size_t *p_src_len, char *dest, size_t *p_dest_len);

/// @brief convert utf16 code to utf8 code
/// @param[in] c16 utf16 code
/// @param[out] c8 utf8 code
/// @param[out] p_c8_len length of utf8 code
void Utf16ToUtf8(uint16_t c16, char *c8, size_t *p_c8_len);

/// @brief convert utf8 code to utf16 code
/// @param[in] c8 utf8 code
/// @param[in] c8_len length of utf8 code
/// @param[out] p_c16 utf16 code
void Utf8ToUtf16(const char *c8, size_t c8_len, uint16_t *p_c16);

/// @brief check whether the following 2 bytes are gbk encoded
/// @param[in] src start pos to check
/// @ret true:is gbk encoded
bool IsGbkCode(const char *src);

/// @brief get the utf8 code length
/// @param[in] src start pos to check
/// @param[out] p_len length to get
/// @ret 0:is utf8 encoded, -1:failure
int GetUtf8Len(const char *src, size_t *p_len);

/// split utf8 character into a list
/// @param[in] utf8_str The utf8 string to split
/// @param[out] p_list The split list
int SplitUtf8(const std::string &utf8_str, std::vector<std::string> *p_list);
}}}}   ///< end of namespace ganji::util::encoding::GbkUtf8Conv
#endif  ///< _GANJI_UTIL_ENCODING_GBK_UTF8_H_
