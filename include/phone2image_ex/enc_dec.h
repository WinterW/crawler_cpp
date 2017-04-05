/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/phone2image/enc_dec.h
 * @namespace ganji::crawler::phone2image::enc_dec
 * @version 1.0
 * @author  lisizhong
 * @date    2011-09-28
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_ENC_DEC_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_ENC_DEC_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <openssl/des.h>
#include <string>
#include <sstream>
#include <map>

using std::string;
using std::ostringstream;
using std::map;

namespace ganji { namespace crawler { namespace phone2image {
class ImgConfigMgr;

const int kDataLen = 100;
/// 字段间的分隔符
const char kDelim = '=';
const int kErrBufLen = 1024;
/// 校验码
const int kChecksumBase = 1000;
const int kChecksumLen = 3;

const char kOrigStr[] = "+/=";
const char kReplacedStr[] = "-._";

/**
 * @class EncDec
 * @brief 号码加解密算法
 */
class EncDec {
 public:
  EncDec() {
  }

  int Init(ImgConfigMgr *p_img_config_mgr);

  /// @brief 编码号码图片id
  /// @param[in] phone 号码
  /// @param[in] style 样式
  /// @param[in] level 封禁级别
  /// @param[in] cur_time 当前时间戳
  /// @param[out] p_img_id 号码图片id
  /// @return -1:failure 0:success
  int EncodeParams(const string &phone,
                   const string &style,
                   const string &domain,
                   const string &cat_major,
                   int level,
                   time_t cur_time,
                   string *p_img_id);


  /// @brief 解析号码图片id
  /// @param[in] img_id 号码图片id
  /// @param[out] param_map 各字段对应的值
  int ParseParams(const string &img_id, const map<string, string> &param_map);

  /// @brief 获取抽取出的字段
  const string &GetPhone() const { return phone_; }
  int GetLevel() const { return level_; }
  const string &GetStyle() const { return style_; }
  const string &GetDomain() const { return domain_; }
  const string &GetCatMajor() const { return cat_major_; }

  /// @brief 加密和编码
  int EncBase(const string &orig_str, string *p_enc);

  /// @brief 解密和解码
  int DecDebase(const string &enc, string *p_orig);

  /// @brief 获取校验码
  void GetChecksum(time_t ts, const string &phone, int level, const string &style, string *p_check_sum);

  char * GetErrBuf() { return err_buf_; }

 private:
  /// @brief 加密号码
  void Encrypt(const char *key, const char *msg, int size, char **encrypted);

  /// @brief 解密号码
  void Decrypt(const char *key, const char *msg, int size, char **decrypted);

  /// @brief 编码base64
  unsigned char * EncodeBase64(const char *str, int orig_len);

  /// @brief 获取base64编码的长度
  void GetBase64Len(int length, int *p_3_count, int *p_mod, int *p_enc_len);

  /// @brief 解码base64的辅助函数，4个字节转换为3个字节
  void DeBase64Byte(unsigned char chuue[4], unsigned char chasc[3]);

  /// @brief 解码base64
  /// @param[in] src 待解码字符串
  /// @param[out] p_result 解码后的字符串
  /// @return -1:failure 0:success
  int DeBase64(const string &src, string *p_result);

  /// @brief 解码base64
  /// @param[in] src 待解码字符串
  /// @param[out] outbuf 解码后的字符串
  /// @return -1:failure else:解码后的字符串长度
  int DeBase64(const string &src, char *outbuf);

  /// @brief 根据映射表替换字符
  /// @param[in] orig_str 待替换字符串
  /// @param[in] rep_map 字符映射表
  /// @param[in] rep_str 字符映射表中的待替换字符
  /// @param[out] p_rep_str 替换后的字符串
  void ReplaceByMap(const string &orig_str,
                    const map<char, char> &rep_map,
                    const char *rep_str,
                    string *p_rep_str);

 private:
  /// base64映射表
  static char base64_table_[64];

  /// 存放base64编码后的数据
  unsigned char enbase64_data_[kDataLen];
  /// 存放base64解码后的数据
  unsigned char debase64_data_[kDataLen];
  /// 存放加密后的数据
  char enc_data_[kDataLen];
  /// 存放解密后的数据
  char dec_data_[kDataLen*2];

  ImgConfigMgr *p_img_config_mgr_;

  /// 提取出的号码
  string phone_;
  /// 提取出的级别
  int level_;
  /// 提取出的类别
  string style_;
  /// 提取出的域名
  string domain_;
  /// 提取出的cat major
  string cat_major_;


  /// 字符替换表，因base64编码中的+ / =在url中被转义，所以需替换为其他字符
  map<char, char> replace_map_;
  /// 替换表的解码表
  map<char, char> rev_replace_map_;

  char err_buf_[kErrBufLen];
};
}}};

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_ENC_DEC_H_
