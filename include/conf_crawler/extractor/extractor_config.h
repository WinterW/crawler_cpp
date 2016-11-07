/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/extractor/extractor_config.h
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-24
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_EXTRACTOR_CONFIG_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_EXTRACTOR_CONFIG_H_

#include <string>

#include "global.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
const char kNbThreadCount[] = "NB_THREAD_COUNT";
const char kSocketTimeout[] = "SOCKET_TIMEOUT";
const char kPersistCount[] = "PERSIST_COUNT";
const char kSvrThreadCount[] = "SVR_THREAD_COUNT";
const char kExtractorPort[] = "EXTRACTOR_PORT";
const char kLinkBaseHost[] = "LINK_BASE_HOST";
const char kLinkBasePort[] = "LINK_BASE_PORT";
const char kTimeSlice[] = "TIME_SLICE";
const char kReqRetryTimes[] = "REQ_RETRY_TIMES";
const char kRelaySvrHost[] = "RELAY_SVR_HOST";
const char kRelaySvrPort[] = "RELAY_SVR_PORT";
const char kTemplatePath[] = "TEMPLATE_PATH";
const char kIsGetTask[] = "IS_GET_TASK";

class ExtractorConfig {
 public:
  ExtractorConfig() { }

  int LoadConfig(const std::string &conf_file);

  void PrintConfig() const;

  int GetNbThreadCount() { return nb_thread_count_; }
  int GetSocketTimeout() { return socket_timeout_; }
  int GetPersistCount() { return persist_count_; }
  int GetSvrThreadCount() { return svr_thread_count_; }
  int GetExtractorPort() { return extractor_port_; }
  const std::string & GetLinkBaseHost() { return link_base_host_; }
  int GetLinkBasePort() { return link_base_port_; }
  int GetTimeSlice() { return time_slice_; }
  int GetReqRetryTimes() { return req_retry_times_; }
  const std::string &RelaySvrHost() { return relay_svr_host_; }
  int RelaySvrPort() { return relay_svr_port_; }
  const std::string &TemplatePath() { return template_path_; }
  bool IsGetTask() { return is_get_task_; }

 private:
  int CheckVal(int val, const std::string &name);

 private:
  int nb_thread_count_;
  int socket_timeout_;
  int persist_count_;
  int svr_thread_count_;
  int extractor_port_;
  std::string link_base_host_;
  int link_base_port_;
  int time_slice_;
  int req_retry_times_;
  std::string relay_svr_host_;
  int relay_svr_port_;
  std::string template_path_;
  bool is_get_task_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ExtractorConfig);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_EXTRACTOR_CONFIG_H_
