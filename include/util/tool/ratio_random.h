/**
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/tool/ratio_random.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-21
 *
 */

#ifndef _GANJI_UTIL_TOOL_RATIO_RANDOM_H_
#define _GANJI_UTIL_TOOL_RATIO_RANDOM_H_

#include <time.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

using std::string;
using std::map;
using std::vector;

namespace ganji { namespace util { namespace tool {
class RatioRandom {
 public:
  struct Cab_ {
    int range_;
    string data_;
  };

 public:
  explicit RatioRandom(double precision = 0.01) : naive_array_(NULL) {
    srand(time(NULL));
    max_ = static_cast<int>(1 / precision);
  }

  RatioRandom(const map<string, double>& conf,
      double precision = 0.01) : naive_array_(NULL) {
    srand(time(NULL));
    max_ = static_cast<int>(1 / precision);
    Seed(conf);
  }

  ~RatioRandom(){
    if (naive_array_ != NULL) {
      delete[] naive_array_;
      naive_array_ = NULL;
    }
  }

  int Seed(const map<string, double>& conf) {
    if (max_ > 1000) {
      return FillCabvec(conf);
    }
    return FillArray(conf);
  }

  const string& Toss() {
    int random = rand() % max_;
    if (naive_array_ != NULL) {
      return naive_array_[random];
    }
    const Cab_& cab = Search_(random);
    return cab.data_;
  }

 private:
  int FillArray(const map<string, double>& conf) {
    naive_array_ = new string[max_];
    int index = 0;
    int range = 0;
    map<string, double>::const_iterator iter = conf.begin();
    for (; iter != conf.end(); ++iter) {
      range += static_cast<int>(iter->second * max_);
      while (index < range) {
        naive_array_[index++] = iter->first;
      }
    }
    if (range != max_) {
      delete[] naive_array_;
      naive_array_ = NULL;
      return -1;
    }
    return 0;
  }

  int FillCabvec(const map<string, double>& conf) {
    range_vec_.clear();
    Cab_ cab;
    cab.range_ = -1;
    map<string, double>::const_iterator iter = conf.begin();
    for (; iter != conf.end(); ++iter) {
      cab.data_ = iter->first;

      int range = static_cast<int>(iter->second * max_);
      if (range > 0) {
        cab.range_ += range;
      }
      range_vec_.push_back(cab);
    }
    if (cab.range_ != max_ - 1) {
      return -1;
    }
    return 0;
  }

  const Cab_& Search_(int num) {
    if (range_vec_.size() < 8) {
      for (vector<Cab_>::const_iterator iter = range_vec_.begin();
          iter != range_vec_.end(); ++iter) {
        if (iter->range_ >= num) {
          return *iter;
        }
      }
    }
    return Binary_Search_(num);
  }

  const Cab_& Binary_Search_(int num) {
    size_t len = range_vec_.size();
    int middle, half;
    int first = 0;
    while (len > 0) {
      half = len >> 1;
      middle = first + half;
      int value = range_vec_[middle].range_;
      if (value > num) {
        len = half;
      } else if (value < num) {
        first = middle + 1;
        len = len - first;
      } else {
        first = middle;
        break;
      }
    }
    return range_vec_[first];
  }

  vector<Cab_> range_vec_;
  string* naive_array_;
  int max_;
};
}}}
#endif  ///< _GANJI_UTIL_TOOL_RATIO_RANDOM_H_
