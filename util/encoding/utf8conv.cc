/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/utf8conv.cc
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-23
 * 
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "utf8conv.h"

#include <iconv.h>
#include <locale.h>
#include <string>

#include "util/thread/mutex.h"

using std::string;
using std::vector;
using std::pair;
using ganji::util::thread::Mutex;
using ganji::util::thread::MutexGuard;


namespace ganji { namespace util { namespace encoding { namespace Utf8Conv {

static Mutex kUTF8Lock;

string Utf8ToGBK(const std::string &str_utf8) {
  MutexGuard lock(&kUTF8Lock);
  string str_ret;
  char *psz_buf = new char[str_utf8.length()*5 + 1];
  if (psz_buf == NULL)
    return str_utf8;
  const char* psz_utf8 = str_utf8.c_str();
  int iutf8_len = str_utf8.size();
  char *psz_locale = psz_buf;
  int ilocale_len = str_utf8.size()*5;
  int iwrite_bytes = 0;
  static int bSetLocale = 0;
  if (!bSetLocale) {
    bSetLocale = 1;
    setlocale(LC_ALL, "");
  }
  iconv_t cd = iconv_open("gbk", "UTF8");
  if (cd != (iconv_t)-1) {
    char *inbuf = const_cast<char *>(psz_utf8);
    size_t inbytesleft = iutf8_len;
    char *outbuf = psz_locale;
    size_t outbytesleft = ilocale_len;
    int iconv_ret;
    // utf8 二进制 第一个字节不会以10开头 0xc0; 其它字节都以10开头
    while (inbytesleft > 0) {
      while ((inbytesleft > 0) && (*inbuf != '\0') && ((* (unsigned char *)inbuf & 0xc0) == 0x80)) {  // 0xc0 == 11000000b ; 0x80 == 10000000b
        // skip rot
        ++inbuf;
        --inbytesleft;
      }
      if (inbytesleft <= 0 || (*inbuf == '\0'))
        break;
      iconv_ret = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
      if (iconv_ret < 0) {
        ++inbuf;
        --inbytesleft;
      }
    }
    iwrite_bytes = outbuf - psz_locale;
    iconv_close(cd);
  }
  psz_locale[iwrite_bytes] = 0;
  str_ret = psz_locale;
  delete[] psz_buf;
  if (str_ret.empty())
    str_ret = str_utf8;
  return str_ret;
}

string GBKToUtf8(const std::string &str_locale) {
  MutexGuard lock(&kUTF8Lock);
  char *psz_buf = new char[str_locale.size()*5+1];
  if (psz_buf == NULL)
    return str_locale;
  string str_ret;
  const char * psz_locale = str_locale.c_str();
  int ilocale_len = str_locale.size();
  char *psz_utf8 = psz_buf;
  int iutf8_len = str_locale.size()*5;
  int iwrite_bytes = 0;
  static int bSetLocale = 0;
  if (!bSetLocale) {
    bSetLocale = 1;
    setlocale(LC_ALL, "");
  }
  iconv_t cd = iconv_open("UTF8", "gbk");
  if (cd != (iconv_t)-1) {
    char *inbuf = const_cast<char *>(psz_locale);
    size_t inbytesleft = ilocale_len;
    char *outbuf = psz_utf8;
    size_t outbytesleft = iutf8_len;
    int iconv_ret;
    while (inbytesleft > 0) {
      iconv_ret = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
      if (iconv_ret < 0) {
        ++inbuf;
        --inbytesleft;
        // skip rot bytes
        while ((inbytesleft > 0) && (*inbuf != '\0')) {
          // 不在GBk编码空间外 必定为ascii 雪崩停止
          if (*(unsigned char *)inbuf < 0x40) {
            break;
          }
          if ((*(unsigned char *)(inbuf+1) != '\0') && (inbytesleft > 2)) {
            if (((*(unsigned char *)inbuf) < 0x80) && ( (*(unsigned char *)inbuf) >= 0x40)) {
              // 其实落在0x80-ox40之前 必定为中文第二个字节或者英文，直接取下一个字节
              ++inbuf;
              --inbytesleft;
              break;
            }
          }
          ++inbuf;
          --inbytesleft;
        }
      }
    }
    iwrite_bytes = outbuf - psz_utf8;
    iconv_close(cd);
  }
  psz_utf8[iwrite_bytes] = 0;
  str_ret = psz_utf8;
  delete[] psz_buf;
  if (str_ret.empty())
    str_ret = str_locale;
  return str_ret;
}

void Utf8ToVector(const string &str , vector<pair<string,size_t> > *pvec){
  if (NULL == pvec)
    return;

  vector<pair<string,size_t> > &vec = *pvec;
  string cur="";
  bool bOne=false;
  size_t pos=0;
  for(size_t i = 0 ; i < str.length(); i ++){
    if((i!=0) && (((str[i]&0xc0)!=0x80 ) ) ){
      bOne=true;
    }
    if(bOne){
      vec.push_back(make_pair(cur,pos));
      pos += cur.length();
      cur="";
      bOne=false;
    }
    cur+=str[i];
    if( (i==(str.length()-1)) ){
      vec.push_back(make_pair(cur,pos));
      pos += cur.length();
    }
  }
};

} } } }   // end of namespace ganji::util::encoding::Utf8Conv
