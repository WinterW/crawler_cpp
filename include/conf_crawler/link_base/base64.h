/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/base64.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-15
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE64_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE64_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <openssl/des.h>
#include <string>
#include <sstream>
#include <map>
#include "conf_crawler/link_base/struct_def.h"

using std::string;
using std::ostringstream;
using std::map;

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
const int kDataLen = 100;

/**
 * @class Base64
 * @brief base64编码
 */
class Base64 {
 public:
  Base64() {
  }

  /// @brief 编码base64
  /// @param[in] orig_str 待编码的原始字符串
  /// @param[in] orig_len The length for the orig_str
  /// @param[out] p_enc 编码后的字符串
  void EncodeBase64(const char *orig_str, size_t orig_len, std::string *p_enc);

  /// @brief 编码base64
  /// @param[in] orig_str 待编码的原始字符串
  /// @param[out] p_enc 编码后的字符串
  void EncodeBase64(const std::string &orig_str, std::string *p_enc);

  /// @brief 解码base64
  /// @param[in] src 待解码字符串
  /// @param[out] p_result 解码后的字符串
  /// @return -1:failure 0:success
  int DeBase64(const string &src, string *p_result);

  char * GetErrBuf() { return err_buf_; }

 private:
  /// @brief 获取base64编码的长度
  void GetBase64Len(int length, int *p_3_count, int *p_mod, int *p_enc_len);

  /// @brief 解码base64的辅助函数，4个字节转换为3个字节
  void DeBase64Byte(char chuue[4], char chasc[3]);

  /// @brief 解码base64
  /// @param[in] src 待解码字符串
  /// @param[out] outbuf 解码后的字符串
  /// @return -1:failure else:解码后的字符串长度
  int DeBase64(const string &src, char *outbuf);

 private:
  /// base64映射表
  static char base64_table_[64];

  char err_buf_[kErrBufLen];
};
}}}};

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE64_H_
