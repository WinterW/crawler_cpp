/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/utime/time_range.h
* @namespace ganji::util::utime
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/


#ifndef GANJI_UTIL_UTIME_TIME_RANGE_H_
#define GANJI_UTIL_UTIME_TIME_RANGE_H_

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

namespace ganji { namespace util { namespace utime {

namespace TimeRange {

  /// 检测一个数是否在Time区间, 有效
  /// @param[in] max time区间最大的边界
  /// @param[in] min time区间最小的边界
  /// @param[in] value 要检测的变量
  /// @return true: value 在time区间       false: value 不在time区间
  bool TimeValidCheck(const int max, const int min, const int value);

  /// 检测当前时间秒数是否在时间段内
  /// @param[in] time_zone 时间段
  /// @param[in] now_time 当前的系统时间对应的秒（时+分+秒）
  /// @return true: now_time在时间段内       false: now_time 不在时间段内
  bool CheckTime(const vector<pair<unsigned int, unsigned int> > &time_zone, const unsigned int now_time);

  /// 检测当前时间星期几是否在时间段内
  /// @param[in] wday_zone 时间段
  /// @param[in] now_wday 当前的系统时间对应的星期几（星期一、星期二等）
  /// @return true: now_time在时间段内       false: now_time 不在时间段内
  bool CheckWday(const vector<pair<unsigned int, unsigned int> > &wday_zone, const unsigned int now_wday);

  /// 检测当前时间是否在推广时段内
  /// @param[in] time_zone 时间段
  /// @param[in] now_time 当前的系统时间对应的秒（时+分+秒）
  /// @return true: now_time在时间段内       false: now_time 不在时间段内
  bool TimeRangeDecide(const vector<pair<unsigned int, unsigned int> > &time_zone, const unsigned int now_time);
}
} } }

#endif  ///< GANJI_UTIL_UTIME_TIME_RANGE_H_
