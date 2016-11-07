/**
 * @Copyright 2011 Ganji Inc.
 * @file    src/ganji/util/tool/d_bit_set.cc
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

#include "d_bit_set.h"
#include <math.h>
#include <assert.h>
#include <string.h>
namespace ganji { namespace util { namespace tool {
static uint8_t g_indicator[8] = {1, 2, 4, 8, 16, 32, 64, 128};
DBitSet::DBitSet() : count_(0), cap_(8), bits_(0) {
  bits_ = new uint8_t[(cap_ - 1) / 8 + 1];
  Reset();
}

DBitSet::~DBitSet() {
  if (bits_)
    delete[] bits_;
}

void DBitSet::Set(int index) {
  assert(index < cap_);
  int pos = index / 8;
  int inner_pos = index % 8;
  if ((bits_[pos] & g_indicator[inner_pos]) == 0) {
    bits_[pos] |= g_indicator[inner_pos];
    ++count_;
  }
}

void DBitSet::Set() {
  memset(bits_, 0xff, (cap_ - 1) / 8 + 1);
  count_ = cap_;
}

void DBitSet::Reset(int index) {
  assert(index < cap_);
  int pos = index / 8;
  int inner_pos = index % 8;
  if (bits_[pos] & g_indicator[inner_pos] != 0) {
    bits_[pos] -= g_indicator[inner_pos];
    --count_;
  }
}

void DBitSet::Reset() {
  memset(bits_, 0x0, (cap_ - 1) / 8 + 1);
  count_ = 0;
}

void DBitSet::Resize(int cap) {
  if (cap > cap_) {
    int new_cap = cap_ + (int)ceil(cap_ * 0.1);
    new_cap = (cap < new_cap) ? new_cap : cap;
    uint8_t *tmp = bits_;
    bits_ = new uint8_t[(new_cap - 1) / 8 + 1];
    memset(bits_, 0, (new_cap - 1) /8 + 1);
    memcpy(bits_, tmp, (cap_ - 1) / 8 + 1);
    cap_ = new_cap;
    delete[] tmp;
  } else {
    cap_ = cap;
  }
}

bool DBitSet::operator [](int index) const {
  assert(index < cap_);
  int pos = index / 8;
  int inner_pos = index % 8;
  return (bits_[pos] & g_indicator[inner_pos]) != 0;
}
} } }    ///< end of namespace ganji::util::tool
