/**
 * @Copyright 2011 Ganji Inc.
 * @file    ganji/util/compress/gzip.cc
 * @namespace ganji::util::compress
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-20
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "gzip.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

using std::string;

namespace ganji { namespace util { namespace compress { namespace Gzip {
int Deflate(const string &src, char *p_def, size_t *p_def_len) {
  int ret, flush;
  unsigned have;
  z_stream strm;
  unsigned char in[kChunk];
  unsigned char out[kChunk];

  /// allocate deflate state
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  int level = Z_DEFAULT_COMPRESSION;
  int method = Z_DEFLATED;
  /// for gzip
  int window_bits = 16+MAX_WBITS;
  int mem_level = 8;
  int strategy = Z_DEFAULT_STRATEGY;

  ret = deflateInit2(&strm, level, method, window_bits, mem_level, strategy);
  if (ret != Z_OK)
    return -1;

  size_t src_pos = 0;
  size_t def_pos = 0;

  /// compress until end of src
  do {
    if (src_pos+kChunk >= src.size()) {
      strm.avail_in = src.size()-src_pos;
      flush = Z_FINISH;
    } else {
      strm.avail_in = kChunk;
      flush = Z_NO_FLUSH;
    }
    memcpy(in, src.c_str()+src_pos, strm.avail_in);
    strm.next_in = in;

    /// run deflate() on input until output buffer not full,
    /// finish compression if all of src has been read in
    do {
      strm.avail_out = kChunk;
      strm.next_out = out;
      /// no bad return value
      ret = deflate(&strm, flush);
      /// state not clobbered
      assert(ret != Z_STREAM_ERROR);
      have = kChunk - strm.avail_out;
      /// the caller should provide more memory
      if (def_pos+have >= *p_def_len) {
        deflateEnd(&strm);
        return -2;
      }
      memcpy(p_def+def_pos, out, have);
      def_pos += have;
    } while (strm.avail_out == 0);
    /// all input will be used
    assert(strm.avail_in == 0);

    src_pos += kChunk;

    /// done when last data in file processed
  } while (flush != Z_FINISH);
  /// stream will be complete
  assert(ret == Z_STREAM_END);

  /// clean up and return
  deflateEnd(&strm);

  *p_def_len = def_pos;

  return 0;
}

int Inflate(const char *p_src, size_t src_len, string *p_inf_str) {
  int ret;
  unsigned have;
  z_stream strm;
  unsigned char in[kChunk];
  unsigned char out[kChunk];

  /// allocate inflate state
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  int window_bits = 16+MAX_WBITS;
  ret = inflateInit2(&strm, window_bits);

  if (ret != Z_OK)
    return -1;

  size_t src_pos = 0;

  /// decompress until deflate stream ends or end of p_src
  do {
    if (src_pos >= src_len)
      break;
    if (src_pos+kChunk >= src_len) {
      strm.avail_in = src_len-src_pos;
    } else {
      strm.avail_in = kChunk;
    }
    memcpy(in, p_src+src_pos, strm.avail_in);
    strm.next_in = in;

    /// run inflate() on input until output buffer not full
    do {
      strm.avail_out = kChunk;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
      /// state not clobbered
      assert(ret != Z_STREAM_ERROR);
      switch (ret) {
        case Z_NEED_DICT:
          /// and fall through
          ret = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
          inflateEnd(&strm);
          return -1;
      }
      have = kChunk - strm.avail_out;
      *p_inf_str += string(reinterpret_cast<char*>(out), 0, have);
    } while (strm.avail_out == 0);

    src_pos += kChunk;
    /// done when inflate() says it's done
  } while (ret != Z_STREAM_END);

  /// clean up and return
  inflateEnd(&strm);
  if (ret == Z_STREAM_END)
    return 0;
  else
    return -1;
}
} } } }   ///< end of namespace ganji::util::compress::Gzip
