/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/encoding/md5.h
 * @namespace     ganji::util::encoding
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 * md5 class
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_ENCODING_MD5_H_
#define _GANJI_UTIL_ENCODING_MD5_H_

#include <string.h>
#include <string>
#include "global.h"

namespace ganji { namespace util { namespace encoding {
/// A struct save md5
/// MD5存储结构体,128位
struct MD5Data {
  /*@{*/
  unsigned int data[4];   ///< md5
  /*@}*/
};

inline bool operator < (const MD5Data &p1, const MD5Data &p2) {
  return memcmp(p1.data, p2.data, 4 * sizeof(int)) < 0;
};

inline bool operator == (const MD5Data &p1, const MD5Data &p2) {
  return memcmp(p1.data, p2.data, 4 * sizeof(int)) == 0;
};

inline bool operator <= (const MD5Data &p1, const MD5Data &p2) {
  return memcmp(p1.data, p2.data, 4 * sizeof(int)) <= 0;
};

/// @class MD5
/// @brief Create md5
/// MD5生成器
class MD5 {
 public:
  /// @fn MD5()
  /// @brief Constructor
  // ! construct a MD5
  MD5();

  /// @fn MD5(const char * md5src)
  /// @brief Construct instance from char *
  /// @param[in] md5src Use to generage md5
  // ! construct a md5src from char *
  explicit MD5(const char *md5src);

  /// @fn MD5(unsigned long *md5src)
  /// @brief Construct instance from 16 bytes md5
  /// @param[in] md5src A 16 bytes md5
  // ! construct a MD5 from a 16 bytes md5
  explicit MD5(uint32_t *md5src);

  /// @fn  GenerateMD5(unsigned char *buffer,size_t bufferlen)
  /// @brief Generate MD5
  /// @param[in] buffer The buffer to generage md5
  /// @param[in] bufferlen Length of buffer
  // ! construct a MD5 from any buffer
  void GenerateMD5(unsigned char *buffer, size_t bufferlen);


  /// @fn operator + (MD5 adder)
  /// @brief Add function
  /// @param[in] adder Another operand
  /// @return The sum of two MD5
  // ! add a other md5
  MD5 AddAnotherMD5(const MD5 &adder) const;


  /// @fn ToString()
  /// @brief Get string of md5
  /// @return The string of md5
  // ! to a std::string
  std::string ToString() const;

  /// save md5 code
  unsigned int m_data_[4];

 private:
  /// declare first
  struct md5_context;

  /// @fn md5_starts(struct md5_context *ctx)
  /// @brief init ctx
  /// @param[out] ctx Generate md5 in ctx
  void md5_starts(struct md5_context *ctx);

  /// @fn md5_process(uint8_t data[64], struct md5_context *ctx)
  /// @brief Update md5 by 64 bytes
  /// @param[in] data[64] Data buffer to be processed
  /// @param[in,out] ctx Update md5 in ctx
  void md5_process(uint8_t data[64], struct md5_context *ctx);

  /// @fn md5_update(struct md5_context *ctx, uint8_t *input, size_t length)
  /// @brief Generate md5
  /// @param[in] input A buffer to generate md5
  /// @param[in] length The length of buffer
  /// @param[out] ctx Generate md5
  void md5_update(uint8_t *input, size_t length, struct md5_context *ctx);

  /// @fn md5_finish(struct md5_context *ctx, uint8_t digest[16])
  /// @brief save md5 to m_data
  /// @param[in] ctx Contain md5
  /// @param[out] digest[16] save md5
  void md5_finish(struct md5_context *ctx, uint8_t digest[16]);

  /// A struct as buffer during generating md5
  struct md5_context {
    /*@{*/
    uint32_t total[2];
    uint32_t state[4];
    uint8_t buffer[64];
    /*@}*/
  };

 private:
  DISALLOW_COPY_AND_ASSIGN(MD5);
};

/// @fn operator ==
/// @brief Equal function
/// @param[in] base the base MD5
/// @param[in] cmper Another MD5 to be compared
/// @return If equal, return true
// ! just if equal
inline bool operator == (const MD5 &base, const MD5 &cmper) {
  return memcmp(cmper.m_data_, base.m_data_, 4 * sizeof(int)) == 0;
};

/// @fn operator <
/// @brief Less function
/// @param[in] base the base MD5
/// @param[in] cmper Another MD5 to be compared
/// @return If less, return true
inline bool operator < (const MD5 &base, const MD5 &cmper) {
  return memcmp(base.m_data_, cmper.m_data_, 4 * sizeof(int)) < 0;
};
} } }   // end of namespace ganji::util::encoding
#endif  // _GANJI_UTIL_ENCODING_MD5_H_
