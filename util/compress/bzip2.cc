/**
 * @Copyright 2011 Ganji Inc.
 * @file    ganji/util/compress/bzip2.cc
 * @namespace ganji::util::compress
 * @version 1.0
 * @author  lisizhong
 * @date    2012-05-16
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "bzip2.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <bzlib.h>

using std::string;

namespace ganji { namespace util { namespace compress { namespace Bzip2 {
size_t CompressedLen(size_t src_len) {
  unsigned int dest_len = static_cast<float>(src_len) * 1.01 + 600;
  return dest_len;
}
int Compress(const string &src, char *p_dest, size_t *p_dest_len) {
  int block_size_100k = 9;
  int verbosity = 0;
  int work_factor = 0;
  int ret = BZ2_bzBuffToBuffCompress(p_dest,
                                     reinterpret_cast<unsigned int *>(p_dest_len),
                                     const_cast<char *>(src.c_str()),
                                     src.size(),
                                     block_size_100k,
                                     verbosity,
                                     work_factor);
  if (ret == BZ_OK)
    return 0;
  return -1;
}

int Decompress(const char *p_src, size_t src_len, string *p_dest_str) {
  int small = 0;
  int verbosity = 0;

  /// try each compression ratio sequentially
  for (int ratio = 5; ratio < 20; ratio++) {
    unsigned int dest_len = src_len * ratio;
    char *dest = new char[dest_len];
    int ret = BZ2_bzBuffToBuffDecompress(dest,
                                         &dest_len,
                                         const_cast<char *>(p_src),
                                         src_len,
                                         small,
                                         verbosity);
    if (ret == BZ_OUTBUFF_FULL) {
      delete []dest;
    } else if (ret == BZ_OK) {
      *p_dest_str = string(dest, dest_len);
      delete []dest;
      return 0;
    } else {
      delete []dest;
      return -1;
    }
  }

  return -1;
}
} } } }   ///< end of namespace ganji::util::compress::Bzip2
