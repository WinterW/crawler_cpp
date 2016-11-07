/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/md5_generator.h
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

#ifndef _GANJI_UTIL_ENCODING_MD5_GENERATOR_H_
#define _GANJI_UTIL_ENCODING_MD5_GENERATOR_H_

#include <string.h>
#include <openssl/md5.h>

#include <string>

#include "global.h"

namespace ganji { namespace util { namespace encoding {
/**
 * @class MD5Generator
 * Md5 generator
 */
class MD5Generator {
 public:
  MD5Generator() {
  }

  /// Generate MD5
  /// @param[in] buffer The buffer to update md5
  /// @param[in] len Length of buffer
  /// @return 0:success -1:failure
  int Generate(const unsigned char *buffer, size_t len);

  /// Generate MD5
  /// @param[in] buffer The buffer to update md5
  /// @return 0:success -1:failure
  int Generate(const std::string &buffer);

  /// Set the digest
  void Digest(const unsigned char *digest) { memcpy(digest_, digest, MD5_DIGEST_LENGTH); }
  /// Get the digest
  unsigned char *Digest() { return digest_; }

  /// Get the string of digest
  /// @param[out] dig The string of digest
  void ToString(std::string *p_str) const;

  /// Get the digest from md5 string
  /// @param[in] md5_str The md5sum
  int FromString(const std::string &md5_str);

 private:
  unsigned char digest_[MD5_DIGEST_LENGTH];

 private:
  DISALLOW_COPY_AND_ASSIGN(MD5Generator);
};
} } }   /// end of namespace ganji::util::encoding
#endif  ///< _GANJI_UTIL_ENCODING_MD5_GENERATOR_H_
