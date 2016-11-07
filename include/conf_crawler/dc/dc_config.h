/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/dc/dc_config.h
 * @namespace ganji::crawler::conf_crawler::dc
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DC_DC_CONFIG_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DC_DC_CONFIG_H_

#include <string>
#include <vector>

#include "global.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dc {
const char kDcHost[] = "DC_HOST";
const char kDcPort[] = "DC_PORT";
const char kLinkBaseHost[] = "LINK_BASE_HOST";
const char kLinkBasePort[] = "LINK_BASE_PORT";
const char kNbThreadCount[] = "NB_THREAD_COUNT";
const char kSocketTimeout[] = "SOCKET_TIMEOUT";
const char kPersistCount[] = "PERSIST_COUNT";
const char kCheckInterval[] = "CHECK_INTERVAL";
const char kDownloadInterval[] = "DOWNLOAD_INTERVAL";
const char kTimeSlice[] = "TIME_SLICE";
const char kRetryTimes[] = "RETRY_TIMES";
const char kCacheExpireInterval[] = "CACHE_EXPIRE_INTERVAL";
const char kDownloadFailLimit[] = "DOWNLOAD_FAIL_LIMITS";
const char kDownloadTimeout[] = "DOWNLOAD_TIME_OUT";
const char kGbkUtf16File[] = "GBK_UTF8_FILE";
const char kGetTaskThreadCount[] = "GET_TASK_THREAD_COUNT";
const char kUserAgentFile[] = "USER_AGENT_FILE";
const char kFailedDelayTime[] = "FAILED_DELAY_TIME";

class DcConfig {
 public:
  DcConfig() { }

  int LoadConfig(const std::string &conf_file);

  void PrintConfig() const;

  const std::string &GetDcHost() { return dc_host_; }
  int GetDcPort() { return dc_port_; }
  const std::string &GetLinkBaseHost() { return link_base_host_; }
  int GetLinkBasePort() { return link_base_port_; }
  int GetNbThreadCount() { return nb_thread_count_; }
  int GetSocketTimeout() { return socket_timeout_; }
  int GetPersistCount() { return persist_count_; }
  int GetCheckInterval() { return check_interval_; }
  int GetDownloadInterval() { return download_interval_; }
  int GetTimeSlice() { return time_slice_; }
  int GetRetryTimes() { return retry_times_; }
  int GetCacheExpireInterval() { return cache_expire_interval_; }
  int GetDownloadTimeout() { return download_time_out_; }
  const std::string &GetGbkUtf16File() { return gbk_utf16_file_; }
  int GetTaskThreadCount() { return get_task_thread_count_; }
  void GetUserAgent(std::string *p_ua);
  int GetFailedDelayTime() { return failed_delay_time_; }

 private:
  int CheckVal(int val, const std::string &name);

  /// @brief load ua file
  /// @param[in] ua_file file name
  int LoadUaFile();

 private:
  std::string dc_host_;
  int dc_port_;
  std::string link_base_host_;
  int link_base_port_;
  int nb_thread_count_;
  int socket_timeout_;
  int persist_count_;
  int check_interval_;
  int download_interval_;       ///< download interval for one domain, in seconds
  int time_slice_;              ///< get task interval, in milliseconds
  int retry_times_;             ///< retry times for request to link base
  int cache_expire_interval_;   ///< expire time interval for download cache, in seconds
  int download_time_out_;       ///< default timeout
  std::string gbk_utf16_file_;       ///< GbkUtf8Conv config file
  int get_task_thread_count_;   ///< thread count of get task thread
  std::string ua_file_;         ///< user agent file
  std::vector<std::string> ua_list_;         ///< user agent list
  int failed_delay_time_;       ///< failed task delay download time.

 private:
  DISALLOW_COPY_AND_ASSIGN(DcConfig);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dc

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DC_DC_CONFIG_H_
