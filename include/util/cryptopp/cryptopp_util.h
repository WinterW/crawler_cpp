/** 
 * @Copyright (c) 2011 Ganji Inc.
 * @file          ganji/util/cryptopp/cryptopp_util.h
 * @namespace     ganji::util::cryptopp
 * @version       1.0
 * @author        liubijian
 * @date          2011-04-21
 * 
 * Change Log:
 *
 */
#ifndef _GANJI_UTIL_CRYPTOPP_H_
#define _GANJI_UTIL_CRYPTOPP_H_

#ifndef CRYPTOPP_ENABLE_NAMESPACE_WEAK
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#endif

#include <cryptopp/aes.h>
#include <cryptopp/md5.h>
#include <string>

#include "global.h"

namespace ganji { namespace util { namespace cryptopp { namespace CryptoPPUtil {

// @brief md5加密
// @param [in] in_str : 需要加密的字符串
// @return 加密后的十六进制字符串编码字符串
std::string Md5Encode(const std::string &in_str);

// @brief aes加密
// @param [in] in_str : 需要加密的字符串
// @param [in] key : 密钥
// @return 加密后的十六进制字符串编码字符串
std::string AesEncode(const std::string &in_str, const std::string &key);

// @brief aes解密
// @param [in] in_str : 需要解密的字符串
// @param [in] key : 密钥
// @return 解密后的字符串
std::string AesDecode(const std::string &in_str, const std::string &key);

// @brief 将整数转为16进制的字符串，如果为1位，则前面补0
// @param [in] value : 待转化的整数
// @return 转化后的16进制字符串
std::string HexToStr(byte value);

// @brief 将16进制的字符串转化为整数，如将"a3"=>0xa3
// @param [in] value : 待转化的字符串
// @return 转化后的整数
uint64_t StrToHex(const std::string &value);

} } } }  // end of namespace ganji::util::cryptopp
#endif  // _GANJI_UTIL_CRYPTOPP_H_
