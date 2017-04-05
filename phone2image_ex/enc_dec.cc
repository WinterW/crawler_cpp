/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/phone2image/enc_dec.cc
 * @namespace ganji::crawler::phone2image::enc_dec
 * @version 1.0
 * @author  lisizhong
 * @date    2011-09-28
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "phone2image_ex/enc_dec.h"

#include <assert.h>
#include "util/text/text.h"
#include "phone2image_ex/img_config.h"

namespace Text = ::ganji::util::text::Text;

namespace ganji { namespace crawler { namespace phone2image {
char EncDec::base64_table_[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/',
};

/// Precondition: 对p_img_config_mgr_->GetDesConfig()中参数已做合法性判断
int EncDec::Init(ImgConfigMgr *p_img_config_mgr) {
  p_img_config_mgr_ = p_img_config_mgr;

  replace_map_['+'] = '-';
  replace_map_['/'] = '.';
  replace_map_['='] = '_';

  rev_replace_map_['-'] = '+';
  rev_replace_map_['.'] = '/';
  rev_replace_map_['_'] = '=';

  return 0;
}

int EncDec::EncBase(const string &orig_str, string *p_enc) {
  char *encrypted = enc_data_;

  memset(encrypted, 0, sizeof(enc_data_));

  const string &key = p_img_config_mgr_->GetDesConfig()->key_;
  Encrypt(key.c_str(), orig_str.c_str(), orig_str.size(), &encrypted);

  unsigned char *base64 = EncodeBase64(encrypted, orig_str.size());

  // printf("base64:%s\n", base64);

  // *p_enc = reinterpret_cast<char *>(base64);

  /// 字符替换
  ReplaceByMap(reinterpret_cast<char *>(base64), replace_map_, kOrigStr, p_enc);

  /// 循环左移
  int rotate_step = p_img_config_mgr_->GetDesConfig()->rotate_step_;
  Text::LeftRotate(p_enc, rotate_step);

  return 0;
}

int EncDec::DecDebase(const string &enc, string *p_orig) {
  string debase;
  int ret = DeBase64(enc, &debase);
  if (ret < 0) {
    return -1;
  }

  char *decrypted = dec_data_;
  memset(decrypted, 0, sizeof(dec_data_));

  const string &key = p_img_config_mgr_->GetDesConfig()->key_;
  Decrypt(key.c_str(), debase.c_str(), sizeof(dec_data_), &decrypted);
  int i = 0;
  for ( ; i < static_cast<int>(sizeof(dec_data_)); i++) {
    if (!isdigit(dec_data_[i]) and dec_data_[i] != '_')
      break;
  }
  *p_orig = string(dec_data_, i);

  return 0;
}

void EncDec::GetBase64Len(int length, int *p_3_count, int *p_mod, int *p_enc_len) {
  int length_mod = length % 3;
  length /= 3;
  int str_enc_len = length * 4;
  str_enc_len += ((length_mod == 0) ? 0 : 4);

  *p_mod = length_mod;
  *p_3_count = length;
  *p_enc_len = str_enc_len;
}

unsigned char * EncDec::EncodeBase64(const char *str, int orig_len) {
  int three_count = 0, length_mod = 0, str_enc_len = 0;
  GetBase64Len(orig_len, &three_count, &length_mod, &str_enc_len);

  const unsigned char *ptr = reinterpret_cast<const unsigned char *>(str);
  unsigned char *pptr = enbase64_data_;
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

  return enbase64_data_;
};

void EncDec::DeBase64Byte(unsigned char chuue[4], unsigned char chasc[3]) {
  for (int i = 0; i < 4; i++) {
    unsigned char ch = chuue[i];
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

int EncDec::DeBase64(const string &src, char *outbuf) {
  /// Break when the incoming base64 coding is wrong
  if ((src.size() % 4) != 0) {
    snprintf(err_buf_, kErrBufLen, "debase64:%s failed, not dividable by 4:%lu", src.c_str(), src.size());
    return -1;
  }

  unsigned char in[4];
  unsigned char out[3];

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

int EncDec::DeBase64(const string &src, string *p_result) {
  char *buf = reinterpret_cast<char *>(enbase64_data_);
  int len = DeBase64(src, buf);
  if (len < 0) {
    return -1;
  }
  buf[len] = '\0';
  *p_result = string(buf, len);
  return 0;
}

void EncDec::Encrypt(const char *key, const char *msg, int size, char **encrypted) {
  /// Prepare the key for use with DES_cfb64_encrypt
  DES_cblock key2;
  DES_key_schedule schedule;
  memcpy(key2, key, 8);
  DES_set_odd_parity(&key2);
  DES_set_key_checked(&key2, &schedule);

  /// Encryption occurs here
  int n = 0;
  unsigned char *res = reinterpret_cast<unsigned char *>(*encrypted);
  DES_cfb64_encrypt(reinterpret_cast<const unsigned char *>(msg),
      res, size, &schedule, &key2, &n, DES_ENCRYPT);
}

void EncDec::Decrypt(const char *key, const char *msg, int size, char **decrypted) {
  /// Prepare the key for use with DES_cfb64_encrypt
  DES_cblock key2;
  DES_key_schedule schedule;
  memcpy(key2, key, 8);
  DES_set_odd_parity(&key2);
  DES_set_key_checked(&key2, &schedule);

  /// Encryption occurs here
  int n = 0;
  unsigned char *res = reinterpret_cast<unsigned char *>(*decrypted);
  DES_cfb64_encrypt(reinterpret_cast<const unsigned char *>(msg),
      res, size, &schedule, &key2, &n, DES_DECRYPT);
}

void EncDec::GetChecksum(time_t ts, const string &phone, int level, const string &style, string *p_check_sum) {
  int check_sum_base = p_img_config_mgr_->GetDesConfig()->check_sum_base_;
  int check_sum_len = p_img_config_mgr_->GetDesConfig()->check_sum_len_;
  int phone_num = 0;
  for (int i = 0; i < static_cast<int>(phone.size()); i++) {
    phone_num = (phone_num + static_cast<uint16_t>(phone[i])) % check_sum_base;
  }

  int style_num = 0;
  for (int i = 0; i < static_cast<int>(style.size()); i++) {
    style_num = (style_num + static_cast<uint16_t>(style[i])) % check_sum_base;
  }

  int check = (ts + phone_num + level + style_num) % check_sum_base;
  *p_check_sum = Text::IntToStr(check);
  if (static_cast<int>(p_check_sum->size()) < check_sum_len) {
    *p_check_sum += string(check_sum_len-p_check_sum->size(), '0');
  }
}

int EncDec::EncodeParams(const string &phone,
                         const string &style,
                         const string &domain,
                         const string &cat_major,
                         int level,
                         time_t cur_time,
                         string *p_img_id) {
  /// TODO 因前端有varnish缓存，需保证url固定，所以暂时不使用时间戳和level
  cur_time = 0;
  level = 0;

  string check_sum;
  GetChecksum(cur_time, phone, level, style, &check_sum);
  char clear[100];
  int ret = snprintf(clear, sizeof(clear), "%ld%c%s%c%d%c%s%c%s%c%s%c%s",
                     cur_time, kDelim, phone.c_str(), kDelim,
                     level, kDelim, style.c_str(), kDelim,
                     domain.c_str(), kDelim, cat_major.c_str(), kDelim,
                     check_sum.c_str());
  if (ret >= static_cast<int>(sizeof(clear))) {
    snprintf(err_buf_, sizeof(err_buf_), "EncodeParams failed %d:%ld=%s=%d=%s=%s, too large", ret,
             cur_time, phone.c_str(), level, style.c_str(), check_sum.c_str());
    return -1;
  }

  string orig_str = clear;
  ret = EncBase(orig_str, p_img_id);

  return ret;
}

int EncDec::ParseParams(const string &img_id, const map<string, string> &param_map) {
  /// 循环左移n-step，恢复左移前字符串
  string orig_str = img_id;
  int rotate_step = p_img_config_mgr_->GetDesConfig()->rotate_step_;
  if (static_cast<int>(img_id.size()) < rotate_step) {
    snprintf(err_buf_, sizeof(err_buf_), "ParseParams failed img_id:%s < %d",
             img_id.c_str(), rotate_step);
    return -1;
  }
  Text::LeftRotate(&orig_str, img_id.size()-rotate_step);

  /// 字符替换
  ReplaceByMap(orig_str.c_str(), rev_replace_map_, kReplacedStr, &orig_str);

  /// base64解码
  if ((int)orig_str.size()/4*3 >= kDataLen)
  {
    return -1;
  }

  string debase;
  int ret = DeBase64(orig_str, &debase);
  if (ret < 0) {
    return -1;
  }


  char *decrypted = dec_data_;
  memset(decrypted, 0, sizeof(dec_data_));

  /// 解密
  const string &key = p_img_config_mgr_->GetDesConfig()->key_;
  Decrypt(key.c_str(), debase.c_str(), sizeof(dec_data_), &decrypted);
  int i = 0;
  char *pos = dec_data_;
  char *next_pos = strchr(dec_data_, kDelim);
  time_t ts = 0;
  string phone;
  int level = 0;
  string style, domain, cat_major;
  const int kSegCount = 6;
  while (i < kSegCount && next_pos) {
    string val = string(pos, next_pos - pos);
    if (i == 0) {
      ts = Text::StrToInt64(val);
    } else if (i == 1) {
      phone = val;
    } else if (i == 2) {
      level = Text::StrToInt(val);
    } else if (i == 3) {
      style = val;
    } else if (i == 4) {
      domain = val;
    } else if (i == 5) {
      cat_major = val;
    }

    pos = next_pos+1;
    next_pos = strchr(pos, kDelim);
    i++;
  }

  if (i < kSegCount) {
    snprintf(err_buf_, kErrBufLen, "decrypted str:%s invalid, %d delim", decrypted, i);
    return -1;
  }

  /// TODO 因前端有varnish缓存，需保证url固定，所以暂时不使用时间戳和level
  // time_t now = time(NULL);
  // if (now < ts) {
  //   snprintf(err_buf_, kErrBufLen, "invalid ts:%lu > now:%lu", ts, now);
  //   return -1;
  // }
  // int diff = now - ts;
  // int time_limit = p_img_config_mgr_->GetDesConfig()->time_limit_;
  // if (diff > time_limit) {
  //   snprintf(err_buf_, kErrBufLen, "invalid ts:%lu+%d < now:%lu", ts, time_limit, now);
  //   return -1;
  // }

  bool valid_check = true;
  int check_sum_len = p_img_config_mgr_->GetDesConfig()->check_sum_len_;
  for (char *p = pos; p < pos+check_sum_len; p++) {
    if (!isdigit(*p)) {
      valid_check = false;
      break;
    }
  }

  if (!valid_check) {
    snprintf(err_buf_, kErrBufLen, "checksum:%s invalid, #digit<3", string(pos, 3).c_str());
    return -1;
  }

  string check_sum_str = string(pos, check_sum_len);
  string orig_check_sum;
  GetChecksum(ts, phone, level, style, &orig_check_sum);
  if (check_sum_str != orig_check_sum) {
    snprintf(err_buf_, kErrBufLen, "checksum:%s invalid, !=%s", check_sum_str.c_str(), orig_check_sum.c_str());
    return -1;
  }

  phone_ = phone;
  level_ = level;
  style_ = style;
  domain_ = domain;
  cat_major_ = cat_major;

  return 0;
}

void EncDec::ReplaceByMap(const string &orig_str,
                          const map<char, char> &rep_map,
                          const char *rep_str,
                          string *p_rep_str) {
  p_rep_str->clear();

  if (orig_str.empty())
    return;

  string &str = *p_rep_str;
  char *pos = const_cast<char *>(orig_str.c_str());
  char *next_pos = NULL;
  while (*pos != '\0' && (next_pos = strpbrk(pos, rep_str)) != NULL) {
    str += string(pos, next_pos - pos);
    map<char, char>::const_iterator it = rep_map.find(*next_pos);
    assert(it != rep_map.end());

    str += it->second;
    pos = next_pos + 1;
  }

  if (*pos != '\0') {
    str += string(pos);
  }
}
}}};
