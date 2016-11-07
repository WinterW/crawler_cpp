/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/thread/thread_statistics.h
* @namespace ganji::util::thread
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
* @effect  监控类，用来监控应用程序中的一些值
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/

#ifndef _GANJI_UTIL_THREAD_THREAD_STATISTICS_H_
#define _GANJI_UTIL_THREAD_THREAD_STATISTICS_H_

#include <pthread.h>
#include <map>
#include <string>

using std::map;
using std::string;
using std::pair;

namespace ganji { namespace util { namespace thread {

class StatisticsManager {
 public:
  
  /// 初始化
  /// @param[in] print_cycle : 打印周期,单位秒, 调用程序需在初始化时调用
  /// @return true:success, false:failed.
  bool Init (const int print_cycle);

  /// 注册一项统计，调用程序需在初始化时调用
  /// @param[in] name: 要注册统计项名, value: 给定的初始值
  void RegisterValueItem(const string &name, const int value);

  /// 注册一项统计(这个统计由两个统计决定)，调用程序需在初始化时调用
  /// @param[in] name: 要注册统计项名, dividend,devisor: 给定的初始值
  void RegisterDevisionItem(const string &name, const double dividend, const int devisor);

  /// 打印所有统计项
  static void* PrintAllStatistics(void *arg);

  /// 清除全部的统计值
  void ClearAllStatistics();


  /// 累加有关联的两项统计
  /// @param[in] name: 要累加统计项名, dividend, divisor: 分别累加的幅度
  /// @return true:success, false:failed.
  bool CumulationDevisionItem(const string &name, const double dividend, const int divisor);

  /// 累加单项统计
  /// @param[in] name: 要累加统计项名, step: 累加的幅度
  /// @return true:success, false:failed.
  bool CumulationValueStatistics(const string &name, const int step);

  /// 更新单项统计
  /// @param[in] name: 要更新统计项名, value: 更新后的值
  /// @return true:success, false:failed.
  bool UpdateValueStatistics(const string &name, const int value);

  static StatisticsManager* GetInstance();

  static StatisticsManager* inst_;
 private:
  pthread_mutex_t statistics_lock_;
  int print_cycle_;
  map<string, int> value_perfcounters_;
  map<string, pair<double, int> > devision_perfcounters_;
};

} } }


#endif ///< _GANJI_UTIL_THREAD_THREAD_STATISTICS_H_
