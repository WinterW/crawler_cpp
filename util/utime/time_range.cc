/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/utime/time_range.cc
* @namespace ganji::util::utime
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/

#include "time_range.h"
#include "util/log/thread_fast_log.h"
#include "util/text/text.h"
#include <sys/time.h>
#include <time.h>

using namespace ganji::util::log::ThreadFastLog;
using namespace ganji::util::text::Text;

namespace ganji { namespace util { namespace utime { namespace TimeRange {

/// 检测一个数是否在Time区间, 有效
bool TimeValidCheck(const int max, const int min, const int value) {
  return (value >= min) && (value <= max) ? true : false ;
}

/// 检测当前时间秒数是否在时间段内
bool CheckTime(const vector<pair<unsigned int, unsigned int> > &time_zone, const unsigned int now_time) {
  int flag = 0;
  for (size_t ui = 0; ui < time_zone.size(); ++ui) {
    unsigned int low_time = time_zone[ui].first;
    unsigned int high_time = time_zone[ui].second;
    if ((now_time >= low_time) && (now_time <= high_time)) { ///< 只要有一个时间段满足就为真
      return true;
    }
    else {
      flag = 1;
    }
  }
  if (flag) {  ///< 全部时间段都不满足，一定进入到循环，即账户设置了时间段
    return false;
  }
  return true; ///< Load全量的时候可能没有时间段，默认时间段是0-24，即满足展现的时间段
}

/// 检测当前时间星期几是否在时间段内
bool CheckWday(const vector<pair<int, int> > &wday_zone, const int now_wday) {
  int flag = 0;
  for (size_t ui = 0; ui < wday_zone.size(); ++ui) {
    int low_wday = wday_zone[ui].first;
    int high_wday = wday_zone[ui].second;
    if ((now_wday >= low_wday) && (now_wday <= high_wday)) { ///< 只要有一个wday段满足就为真
      return true;
    }
    else {
      flag = 1;
    }
  }
  if (flag) { ///< 全部wday段都不满足，一定进入到循环，即账户设置了wday段
    return false;
  }
  return true; ///< Load全量的时候可能没有时间段，默认时间段是1-7，即满足展现的时间段
}

/// 检测当前时间是否在推广时段内
bool TimeRangeDecide(const vector<pair<unsigned int, unsigned int> > &time_zone, const unsigned int now_time) {
  return CheckTime(time_zone, now_time);
}


} } } }///< end of namespace ganji::util::utime::timerange
