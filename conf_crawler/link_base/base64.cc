/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/base64.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-15
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/link_base/base64.h"

#include <assert.h>

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
char Base64::base64_table_[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/',
};

void Base64::GetBase64Len(int length, int *p_3_count, int *p_mod, int *p_enc_len) {
  int length_mod = length % 3;
  length /= 3;
  int str_enc_len = length * 4;
  str_enc_len += ((length_mod == 0) ? 0 : 4);

  *p_mod = length_mod;
  *p_3_count = length;
  *p_enc_len = str_enc_len;
}

void Base64::EncodeBase64(const char *orig_str, size_t orig_len, string *p_enc) {
  int three_count = 0, length_mod = 0, str_enc_len = 0;
  GetBase64Len(orig_len, &three_count, &length_mod, &str_enc_len);

  const char *ptr = orig_str;
  char *pptr = new char[orig_len * 4 / 3 + 10];
  char *p = pptr;
  for (int i = 0; i < three_count; i++) {
    unsigned char ch0 = ptr[0];
    unsigned char ch1 = ptr[1];
    unsigned char ch2 = ptr[2];
    *pptr++ = base64_table_[(ch0 >> 2)];
    *pptr++ = base64_table_[((ch0 & 0x3) << 4) + (ch1 >> 4)];
    *pptr++ = base64_table_[((ch1 & 0xf) << 2) + (ch2 >> 6)];
    *pptr++ = base64_table_[(ch2 & 0x3f)];
    ptr += 3;
  }

  if (length_mod == 1) {
    unsigned char ch0 = ptr[0];
    *pptr++ = base64_table_[(ch0 >> 2)];
    *pptr++ = base64_table_[((ch0 & 0x3) << 4)];
    *pptr++ = '=';
    *pptr++ = '=';
  } else if (length_mod == 2) {
    unsigned char ch0 = ptr[0];
    unsigned char ch1 = ptr[1];
    *pptr++ = base64_table_[(ch0 >> 2)];
    *pptr++ = base64_table_[((ch0 & 0x3) << 4) + (ch1 >> 4)];
    *pptr++ = base64_table_[((ch1 & 0xf) << 2)];
    *pptr++ = '=';
  }

  *pptr = '\0';

  *p_enc = string(p);
  delete []p;
};

void Base64::EncodeBase64(const string &orig_str, string *p_enc) {
  EncodeBase64(orig_str.c_str(), orig_str.size(), p_enc) ;
};

void Base64::DeBase64Byte(char chuue[4], char chasc[3]) {
  for (int i = 0; i < 4; i++) {
    char ch = chuue[i];
    /// 'A'-'Z' -> 0-25
    if (ch >= 'A' && ch <= 'Z')
      *(chuue+i) -= 65;
    /// 'a'-'z' -> 26-51
    else if (ch >= 'a' && ch <= 'z')
      *(chuue+i) -= 71;
    /// '0'-'9' -> 52-61
    else if (ch >= '0' && ch <= '9')
      *(chuue+i) += 4;
    /// + -> 62
    else if (ch == '+')
      *(chuue+i) = 62;
    /// / -> 63
    else if (ch == '/')
      *(chuue+i) = 63;
    /// = -> 0  Note: 'A'和'='都对应了0
    else if (ch == '=')
      *(chuue+i) = 0;
  }
  int k = 2;
  for (int i = 0; i < 3; i++) {
    *(chasc+i) = (chuue[i] << k);
    k += 2;
    char t = chuue[i+1] >> (8-k);
    *(chasc+i) |= t;
  }
}

int Base64::DeBase64(const string &src, char *outbuf) {
  /// Break when the incoming base64 coding is wrong
  if ((src.size() % 4) != 0) {
    snprintf(err_buf_, kErrBufLen, "debase64:%s failed, not dividable by 4:%lu", src.c_str(), src.size());
    return -1;
  }

  char in[4];
  char out[3];

  size_t blocks = src.size()/4;
  for (size_t i = 0; i < blocks; i++) {
    in[0] = src[i*4];
    in[1] = src[i*4+1];
    in[2] = src[i*4+2];
    in[3] = src[i*4+3];
    DeBase64Byte(in, out);
    outbuf[i*3] = out[0];
    outbuf[i*3+1] = out[1];
    outbuf[i*3+2] = out[2];
  }
  int length = src.size() / 4 * 3;
  if (src[src.size()-1] == '=') {
    length--;
    if (src[src.size()-2] == '=') {
      length--;
    }
  }
  return length;
}

int Base64::DeBase64(const string &src, string *p_result) {
  char *buf = new char[src.size()];
  int len = DeBase64(src, buf);
  if (len < 0) {
    delete []buf;
    return -1;
  }

  buf[len] = '\0';
  *p_result = string(buf, len);
  delete []buf;

  return 0;
}
}}}};
