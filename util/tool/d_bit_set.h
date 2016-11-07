/**
 * @Copyright 2011 Ganji Inc.
 * @file    src/ganji/util/tool/d_bit_set.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  jiafazhang
 * @date    2011-01-18
 *
 * Define a class like std::bitset, but with dynamic bitset size
 *
 * Change Log:
 *
 */
#ifndef _GANJI_UTIL_TOOL_D_BIT_SET_H_
#define _GANJI_UTIL_TOOL_D_BIT_SET_H_
#include <stdint.h>

namespace ganji { namespace util { namespace tool {
/*
 * @class DBitSet
 *
 * @desc A class like std::bitset, but with dynamic bitset size
 */

class DBitSet {
 public:
  /// @brief Constructor
  DBitSet();

  /// @brief Destructor
  ~DBitSet();

  void Set(int index);
  void Set();

  /// @brief Get the next line of data in the file
  /// @param[in] index The number of fields in one line
  /// @return Get the data successfully or not
  void Reset(int index);
  void Reset();

  bool operator [](int index) const;

  void Resize(int cap);

  bool Any() const {
    return count_ > 0;
  }

  int Count() const {
    return count_;
  }
 private:
  int count_;
  int cap_;
  uint8_t *bits_;
};
} } }    ///< end of namespace ganji::util::tool
#endif   ///< _GANJI_UTIL_TOOL_D_BIT_SET_H_
