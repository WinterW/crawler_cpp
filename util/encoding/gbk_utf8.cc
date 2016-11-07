/** 
 * @Copyright(c)  2012 Ganji Inc.
 * @file          ganji/util/encoding/gbk_utf8.h
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        lisizhong
 * @date          2010-03-07
 * 
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */


#include "gbk_utf8.h"

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string>
#include <vector>

#include "util/text/text.h"

namespace ganji { namespace util { namespace encoding { namespace GbkUtf8Conv {
using std::string;
using std::vector;

namespace Text = ::ganji::util::text::Text;

static uint16_t gbk_utf16_arr_[65535];
static uint16_t utf16_gbk_arr_[65535];

int Init(const string &file_name) {
  FILE *fp = fopen(file_name.c_str(), "r");
  if (!fp) {
    char err_buf[1024];
    snprintf(err_buf, sizeof(err_buf), "open:%s failed:%s",
            file_name.c_str(),
            strerror(errno));
    return -1;
  }

  memset(gbk_utf16_arr_, kPadding, sizeof(gbk_utf16_arr_));
  memset(utf16_gbk_arr_, kPadding, sizeof(utf16_gbk_arr_));

  size_t n = 0;
  char *p_line = NULL;
  vector<string> item_list;

  /// gbk_code utf16_code zh_CN
  while (getline(&p_line, &n, fp) != -1) {
    p_line[n-1] = '\0';
    Text::Segment(p_line, " ", &item_list);
    if (item_list.size() < 3)
      continue;

    int64_t g_value = strtol(item_list[0].c_str(), 0, 16);
    if (g_value == LONG_MAX ||
        g_value >= USHRT_MAX)
      continue;

    uint16_t g_code = static_cast<uint16_t>(g_value);

    if (item_list[1].size() <= 2)
      continue;
    int64_t u_value = strtol(item_list[1].c_str()+2, 0, 16);
    if (u_value == LONG_MAX ||
        u_value >= USHRT_MAX)
      continue;

    uint16_t u_code = static_cast<uint16_t>(u_value);

    gbk_utf16_arr_[g_code] = u_code;
    utf16_gbk_arr_[u_code] = g_code;
  }
  if (p_line)
    free(p_line);
  fclose(fp);

  return 0;
}

bool IsUtf8Str(const string &src) {
  size_t src_len = src.size();
  size_t src_pos = 0;
  while (src_pos < src_len) {
    size_t utf8_len = 0;
    int ret = GetUtf8Len(src.c_str()+src_pos, &utf8_len);
    /// invalid utf8
    if (ret < 0)
      break;

    /// exceed the max length of src
    if (src_pos + utf8_len > src_len)
      break;
    src_pos += utf8_len;
  }

  if (src_pos < src_len)
    return false;
  else
    return true;
}

bool IsGbkStr(const string &src) {
  size_t src_len = src.size();
  size_t src_pos = 0;

  while (src_pos < src_len) {
    if (IsGbkCode(src.c_str()+src_pos)) {
      /// must be 2 bytes, exceed the range of src
      if (src_pos + 2 > src_len) {
        break;
      }
      src_pos += 2;
    } else {
      /// invalid char
      if (src[src_pos] < 0)
        break;
      /// exceed the src's range
      if (src_pos + 1 > src_len)
        break;
      src_pos++;
    }
  }

  if (src_pos < src_len)
    return false;
  else
    return true;
}

int GbkToUtf8(const std::string &src, std::string *p_dest) {
  size_t src_len = src.size();
  size_t dest_len = src_len * 3 + 1;
  char *dest = new char[dest_len];

  GbkToUtf8(src.c_str(), &src_len, dest, &dest_len);
  int ret = 0;
  if (src_len != src.size())
    ret = -1;
  else
    *p_dest = dest;
  delete []dest;

  return ret;
}

int Utf8ToGbk(const std::string &src, std::string *p_dest) {
  size_t src_len = src.size();
  size_t dest_len = src_len + 1;
  char *dest = new char[dest_len];

  Utf8ToGbk(src.c_str(), &src_len, dest, &dest_len);
  int ret = 0;
  if (src_len != src.size())
    ret = -1;
  else
    *p_dest = dest;
  delete []dest;

  return ret;
}

void GbkToUtf8(const char *src, size_t *p_src_len, char *dest, size_t *p_dest_len) {
  size_t src_len = *p_src_len;
  size_t dest_len = *p_dest_len;
  uint16_t g_code = 0;
  uint16_t u_code = 0;
  char utf8_arr[9];
  size_t utf8_len;
  size_t src_pos = 0;
  size_t dest_pos = 0;

  while (true) {
    if (IsGbkCode(src + src_pos)) {
      /// must be 2 bytes, exceed the range of src
      if (src_pos + 2 > src_len) {
        break;
      }
      g_code = static_cast<unsigned char>(src[src_pos]);
      g_code <<= 8;
      g_code += static_cast<unsigned char>(src[src_pos+1]);
      u_code = gbk_utf16_arr_[g_code];

      /// no coresponding code
      if (kPadding == u_code) {
        break;
      }

      /// convert utf16 to utf8
      Utf16ToUtf8(u_code, utf8_arr, &utf8_len);
      /// exceed the dest's range
      if (dest_pos + utf8_len > dest_len)
        break;
      memcpy(dest + dest_pos, utf8_arr, utf8_len);

      src_pos += 2;
      dest_pos += utf8_len;
    } else {
      /// invalid char
      if (src[src_pos] < 0)
        break;
      /// exceed the src's range
      if (src_pos + 1 > src_len)
        break;
      /// exceed the dest's range
      if (dest_pos + 1 > dest_len)
        break;
      dest[dest_pos] = src[src_pos];
      src_pos++;
      dest_pos++;
    }
  }

  *p_src_len = src_pos;
  if (dest_pos < dest_len)
    dest[dest_pos] = '\0';
  *p_dest_len = dest_pos;
}

void Utf8ToGbk(const char *src, size_t *p_src_len, char *dest, size_t *p_dest_len) {
  size_t src_len = *p_src_len;
  size_t dest_len = *p_dest_len;
  uint16_t g_code = 0;
  uint16_t u_code = 0;
  size_t utf8_len = 0;
  size_t src_pos = 0;
  size_t dest_pos = 0;
  while (true) {
    /// end
    if (src_pos >= src_len)
      break;
    int ret = GetUtf8Len(src+src_pos, &utf8_len);
    /// invalid utf8
    if (ret < 0)
      break;

    /// exceed the max length of src
    if (src_pos + utf8_len > src_len)
      break;
    if (1 == utf8_len) {
      /// exceed the max length
      if (dest_pos + 1 > dest_len)
        break;
      dest[dest_pos] = src[src_pos];
      dest_pos++;
      src_pos++;
      continue;
    }

    /// not gbk, break
    if (utf8_len > 3)
      break;

    /// get utf16 code
    Utf8ToUtf16(src + src_pos, utf8_len, &u_code);
    /// get g_code
    g_code = utf16_gbk_arr_[u_code];

    /// exceed the max length
    if (dest_pos + 2 > dest_len)
      break;

    if (0 != g_code) {
      dest[dest_pos] = static_cast<char>(g_code>>8);
      dest[dest_pos+1] = static_cast<char>(g_code);
      dest_pos +=2;
    }
    src_pos += utf8_len;
  }

  *p_src_len = src_pos;
  if (dest_pos < dest_len)
    dest[dest_pos] = '\0';
  *p_dest_len = dest_pos;
}

void Utf16ToUtf8(uint16_t c16, char *c8, size_t *p_c8_len) {
  ///  UCS-2(Hex)        UTF-8(Binary)
  ///  0000 - 007F       0xxxxxxx
  ///  0080 - 07FF       110xxxxx 10xxxxxx
  ///  0800 - FFFF       1110xxxx 10xxxxxx 10xxxxxx

  if (c16 < 0x80) {
    *p_c8_len = 1;
    c8[0] = static_cast<char>(c16);
  } else if (c16 < 0x800) {
    c8[0] = static_cast<char>(0xC0) | ((c16>>6)&0x1F);
    c8[1] = static_cast<char>(0x80) | (c16 & 0x3F);
    *p_c8_len = 2;
  } else {
    c8[0] = static_cast<char>(0xE0) | ((c16>>12)&0xF);
    c8[1] = static_cast<char>(0x80) | ((c16>>6)& 0x3F);
    c8[2] = static_cast<char>(0x80) | (c16 & 0x3F);
    *p_c8_len = 3;
  }
}

void Utf8ToUtf16(const char *c8, size_t c8_len, uint16_t *p_c16) {
  if (1 == c8_len) {
    *p_c16 = c8[0];
  } else if (2 == c8_len) {
    *p_c16 = (c8[0]&0x1F)<<6;
    *p_c16 += c8[1]&0x3F;
  } else {
    *p_c16 = (c8[0]&0x0F)<<12;
    *p_c16 += (c8[1]&0x3F)<<6;
    *p_c16 += (c8[2]&0x3F);
  }
}

/// precondition: caller should guarantee that src and src+1 are valid positions
bool IsGbkCode(const char *src) {
  unsigned char ch0 = src[0];
  unsigned char ch1 = src[1];
  if ((ch0 >=0x81) &&
      (ch0 <=0xFE) &&
      (ch1 >=0x40) &&
      (ch1 <=0xFE))
    return true;
  return false;
}

int GetUtf8Len(const char *src, size_t *p_len) {
  unsigned char c = src[0];
  if ((c & 0x80) == 0) {
    *p_len = 1;
    return 0;
  }

  for (size_t i = 1; i < 6; i++) {
    c <<= 1;
    if ((c & 0x80) == 0) {
      *p_len = i;
      break;
    }
    /// error
    if ((src[i] & 0xC0) != 0x80) {
      *p_len = 0;
      return -1;
    }
  }
  return 0;
}

int SplitUtf8(const string &utf8_str, vector<string> *p_list) {
  p_list->clear();

  size_t start_pos = 0;
  size_t utf8_len = 0;

  while (start_pos < utf8_str.size()) {
    int ret = GetUtf8Len(utf8_str.c_str()+start_pos, &utf8_len);
    if (ret < 0) {
      return -1;
    }
    string utf8_ch = utf8_str.substr(start_pos, utf8_len);
    p_list->push_back(utf8_ch);
    start_pos += utf8_len;
  }

  return 0;
}
}}}}   ///< end of namespace ganji::util::encoding::GbkUtf8Conv

