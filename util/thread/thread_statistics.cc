/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/thread/thread_statistics.cc
* @namespace ganji::util::thread
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/
#include "thread_statistics.h"
#include "util/log/thread_fast_log.h"
#include <time.h>
#include <unistd.h>

using ganji::util::thread::StatisticsManager;
using namespace ganji::util::log::ThreadFastLog;

StatisticsManager* StatisticsManager::inst_ = NULL;

StatisticsManager* StatisticsManager::GetInstance() {
  if (NULL == inst_) {
    inst_ = new (std::nothrow) StatisticsManager();
    if (NULL == inst_){
      WriteLog(kLogFatal, "new StatisticsManager err.");
      return false;
    }
  }
  return inst_;
}

bool StatisticsManager::Init(const int print_cycle) {
  if (pthread_mutex_init(&statistics_lock_, NULL) != 0) {
     WriteLog(kLogFatal, "pthread_mutex_init statistics_lock_ err");
     return false;
  }
  print_cycle_ = print_cycle;
  return true;
}

void StatisticsManager::RegisterValueItem(const string &name, const int value) {
  value_perfcounters_.insert(make_pair(name,value));
}

void StatisticsManager::RegisterDevisionItem(const string &name, const double dividend, const int divisor) {
  pair<double, int> item;
  item.first = dividend;
  item.second = divisor;
  devision_perfcounters_.insert(make_pair(name,item));
}

bool StatisticsManager::UpdateValueStatistics(const string &name, const int value) {
  map<string, int>::iterator it = value_perfcounters_.find(name);
  if (it == value_perfcounters_.end()) {
    WriteLog(kLogWarning, "in UpdateValueStatistics, %s no register", name.c_str());
    return false;
  }
  else {
    it->second = value;
    return true;
  }
}

bool StatisticsManager::CumulationValueStatistics(const string &name, const int step) {
  map<string, int>::iterator it = value_perfcounters_.find(name);
  if (it == value_perfcounters_.end()) {
    WriteLog(kLogWarning, "in CumulationValueStatistics %s no register", name.c_str());
    return false;
  }
  else {
    it->second += step;
    return true;
  }
}

bool StatisticsManager::CumulationDevisionItem(const string &name, const double dividend_step, const int divisor_step) {
  map<string, pair<double, int> >::iterator it = devision_perfcounters_.find(name);
  if (it == devision_perfcounters_.end()) {
    WriteLog(kLogWarning, "in CumulationDevisionItem %s no register", name.c_str());
    return false;
  }

  pthread_mutex_lock(&statistics_lock_);
  it->second.first += dividend_step;
  it->second.second += divisor_step;
  pthread_mutex_unlock(&statistics_lock_);
}

void* StatisticsManager::PrintAllStatistics(void *arg) {
  StatisticsManager *sm = StatisticsManager::GetInstance();
  while(true) {
    sleep(sm->print_cycle_);
    map<string, int>::iterator value_it = sm->value_perfcounters_.begin();
    for (; value_it != sm->value_perfcounters_.end(); ++value_it) {
      WriteLog(kLogNotice, "perf counter -  %s : %d", 
                            value_it->first.c_str(), value_it->second);
    }
    map<string, pair<double, int> >::iterator div_it = sm->devision_perfcounters_.begin();
    for (; div_it != sm->devision_perfcounters_.end(); ++div_it) {
      double div = div_it->second.second > 0 ? div_it->second.first / div_it->second.second : 0;
      WriteLog(kLogNotice, "perf counter - %s : %f, %d ---- %f", 
                           div_it->first.c_str(), div_it->second.first, div_it->second.second, div);
    }
    sm->ClearAllStatistics();
  }
  return NULL;
}

void StatisticsManager::ClearAllStatistics() {
  map<string, int>::iterator value_it = value_perfcounters_.begin();
  map<string, pair<double, int> >::iterator div_it = devision_perfcounters_.begin();
  pthread_mutex_lock(&statistics_lock_);
  for (; value_it != value_perfcounters_.end(); ++value_it) {
    value_it->second = 0;
  }
  for (; div_it != devision_perfcounters_.end(); ++div_it) {
    div_it->second.first = 0;
    div_it->second.second = 0;
  }
  pthread_mutex_unlock(&statistics_lock_);
}
