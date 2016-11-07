/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/http_opt.cc
 * @namespace     ganji::util::net
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "http_opt.h"

#include <string.h>
#include <stdlib.h>
#include <string>

#include "util/text/text.h"
#include "util/encoding/encoding.h"

using std::string;
namespace Text = ganji::util::text::Text;
namespace Encoding = ganji::util::encoding::Encoding;

namespace ganji { namespace util { namespace net { namespace Http {

enum {
  URLCHR_RESERVED = 1,
  URLCHAR_UNSAFE   = 2
};

static const int R = URLCHR_RESERVED;
static const int U = URLCHAR_UNSAFE;
static const int RU =  R|U;
static const unsigned char urlchr_table[256] = {
  U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
  U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
  U,  0,  U,  U,   0,  U,  R,  0,   /* SP  !   "   #    $   %   &   '   */
  0,  0,  0,  R,   0,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
  0,  0,  U,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
  RU,  0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
  0,  0,  0,  U,   U,  U,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
  U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
  0,  0,  0,  U,   U,  U,  U,  U,   /* x   y   z   {    |   }   ~   DEL */
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
};

string EscapeURL(const string &url) {
  string strurl = url;
  string::size_type pos;
  string::size_type t = 0;
  string result = "";
  static const string protocal = "://";
  Text::Trim(&strurl);
  if ((pos = strurl.find(protocal)) != string::npos) {
    t = pos + protocal.size();
    string::size_type path_begin = strurl.find("/", t);
    if (path_begin != string::npos) {
      t = path_begin;
      result = strurl.substr(0, t);
    } else {
      return url;
    }
  }

  for (string::size_type i = t; i < strurl.size(); ++i) {
    unsigned char c = strurl[i];
    if (urlchr_table[c] != URLCHAR_UNSAFE) {
      result += c;
    } else {
      if (c == '%' && i + 2 < url.size() &&
          !urlchr_table[static_cast<unsigned char>(strurl[i + 1])] &&
          !urlchr_table[static_cast<unsigned char>(strurl[i + 2])]) {
        result += c;
        continue;
      } else {
        result += '%';
        result += Encoding::Dec2HexChar(c >> 4);
        result += Encoding::Dec2HexChar(c & 0xf);
      }
    }
  }
  return result;
}

string DeescapeURL(const string &url) {
  string result = "";
  for (string::size_type i = 0; i < url.size(); ++i) {
    char c = url[i];
    if (c != '%') {
      if (c == '+')
        result += ' ';
      else
        result += c;
    } else if ((i + 2) < url.size()) {
      char c1 = url[++i];
      char c0 = url[++i];
      int num = Encoding::HexChar2Dec(c1) * 16 + Encoding::HexChar2Dec(c0);
      result += static_cast<char>(num);
    }
  }
  return result;
}

/// XXX *p_port can be 0 even if return==0
int ParseUrl(const string &url,
    string *p_protocol,
    string *p_domain,
    int *p_port) {
  p_protocol->clear();
  p_domain->clear();
  *p_port = 0;

  const char kPrefix[] = "://";
  const char *pos = strcasestr(url.c_str(), reinterpret_cast<const char*>(kPrefix));
  if (!pos)
    return -1;
  *p_protocol = url.substr(0, pos-url.c_str());

  *p_domain = url.substr(pos-url.c_str()+sizeof(kPrefix)-1);

  int domain_len = p_domain->size();
  for (int i = 0; i < domain_len; i++) {
    if ((*p_domain)[i] == '/') {
      *p_domain = (*p_domain).substr(0, i);
      break;
    } else if ((*p_domain)[i] == ':') {
      string port = (*p_domain).substr(i+1);
      for (int j = i+1; j < domain_len; j++) {
        if ((*p_domain)[j] == '/') {
          port = (*p_domain).substr(i+1, j-i-1);
          break;
        }
      }
      *p_port = atoi(port.c_str());
      *p_domain = (*p_domain).substr(0, i);
      break;
    }
  }

  if (p_domain->empty())
    return -1;
  return 0;
}

int GetUrlDomain(const string &url, string *domain) {
  // format http://xxx.xxx.xx
  const char *http_prefix = "http://";
  if (strncasecmp(url.c_str(), http_prefix, strlen(http_prefix)) != 0) {
    return -1;
  }
  *domain = url.substr(strlen(http_prefix));

  for (int i = 0; i < static_cast<int>(domain->size()); i++) {
    if ((*domain)[i] == '/' || (*domain)[i] == ':') {
      *domain = (*domain).substr(0, i);
      break;
    }
  }
  return 0;
}

int GetMainDomain(const string &url, string *main_domain) {
  string domain;
  if (GetUrlDomain(url, &domain) < 0)
    return -1;

  /// check if is domain is IP, then return
  bool is_ip = true;
  for (int i = 0; i < static_cast<int>(domain.size()); i++) {
    char ch = domain[i];
    if (isalpha(ch)) {
      is_ip = false;
      break;
    }
  }
  if (is_ip)
    return -1;

  /// http://ganji.com
  /// http://ganji.com.cn
  /// http://www.ganji.com
  /// http://www.fang.ganji.com
  /// http://www.ganji.com.cn"
  /// .com .cn .net .org .com.cn
  /// 获得一级域名
  size_t tpos1 = string::npos;
  for (int i = 0; i < static_cast<int>(sizeof(kTopDomain)/sizeof(kTopDomain[0])); i++) {
    tpos1 = domain.find(kTopDomain[i]);
    if (tpos1 != string::npos) {
      break;
    }
  }
  /// 如果是 www.ganji.com  www.ganji.com.cn 这样 就从.com 往前找"."
  if (tpos1 != string::npos) {
    tpos1 -= 1;
    size_t tpos2 = string::npos;
    tpos2 = domain.rfind(".", tpos1);
    if (tpos2 != string::npos) {
      domain = domain.substr(tpos2+1, domain.length());
    }
  } else {
    size_t tpos2 = string::npos;
    tpos2 = domain.rfind(".");
    tpos2 -= 1;
    /// 如果不是www.ganji.uk 这样的域名 就找到 倒数 第二个"."
    tpos2 = domain.rfind("." , tpos2);
    /// 如果得到第二个"."就截取 主域名 ， 否则 本身就是主域名
    if (tpos2 != string::npos) {
      domain = domain.substr(tpos2+1, domain.length());
    }
  }

  if (domain.empty()) {
    return -1;
  } else {
    *main_domain = domain;
    return 0;
  }
}

} } } }   ///< end of namespace ganji::util::net
