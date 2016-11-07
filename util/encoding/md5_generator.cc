/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/md5_generator.cc
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        lisizhong
 * @date          2012-04-23
 * 
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "md5_generator.h"
#include "util/text/text.h"

using std::string;
namespace Text = ganji::util::text::Text;

namespace ganji { namespace util { namespace encoding {
int MD5Generator::Generate(const unsigned char *buffer, size_t len) {
  unsigned char *ret = MD5(buffer, len, digest_);
  if (!ret)
    return -1;
  return 0;
}

int MD5Generator::Generate(const string &buffer) {
  unsigned char *ret = MD5(reinterpret_cast<const unsigned char*>(buffer.c_str()), buffer.size(), digest_);
  if (!ret)
    return -1;
  return 0;
}

void MD5Generator::ToString(string *p_str) const {
  char output[2*MD5_DIGEST_LENGTH];
  for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
    snprintf(output + i * 2, sizeof(output), "%02x", ((unsigned char*)digest_)[i]);
  }

  *p_str = string(output, 0, 2*MD5_DIGEST_LENGTH);
}

int MD5Generator::FromString(const string &md5_str) {
  int len = md5_str.size();
  if (len != 2*MD5_DIGEST_LENGTH)
    return -1;
  memset(digest_, 0, sizeof(MD5_DIGEST_LENGTH));
  for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
    int v1 = Text::FromString<int>(md5_str.substr(2*i, 1), 16);
    int v2 = Text::FromString<int>(md5_str.substr(2*i+1, 1), 16);
    int sum = v1*16 + v2;
    digest_[i] = sum;
  }
  return 0;
}
} } }   // end of namespace ganji::util::encoding
