/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/downloader_config.h
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-26
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_CONFIG_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_CONFIG_H_

#include <string>
#include <vector>

#include "global.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
const char kDelim[] = "|";
const char kDebugMode[] = "DEBUG_MODE";
const char kDcHost[] = "DC_HOST";
const char kDcPort[] = "DC_PORT";
const char kSocketTimeout[] = "SOCKET_TIMEOUT";
const char kPersistCount[] = "PERSIST_COUNT";
const char kCheckInterval[] = "CHECK_INTERVAL";
const char kDownloadInterval[] = "DOWNLOAD_INTERVAL";
const char kTimeSlice[] = "TIME_SLICE";
const char kRetryTimes[] = "RETRY_TIMES";
const char kCacheExpireInterval[] = "CACHE_EXPIRE_INTERVAL";
const char kDownloadFailLimit[] = "DOWNLOAD_FAIL_LIMITS";
const char kDownloadThreadCount[] = "DOWNLOAD_THREAD_COUNT";
const char kUploadRetryTimes[] = "UPLOAD_RETRY_TIMES";
const char kLocalInterface[] = "LOCAL_INTERFACE";
const char kCurlOptMaxRedirs[] = "CURLOPT_MAXREDIRS";
const char kDomainList[] = "DOMAIN_LIST";
const char kIsNetCheck[] = "IS_NET_CHECK";
const char kNetCheckTimeout[] = "NET_CHECK_TIMEOUT";
const char kNetCheckInterval[] = "NET_CHECK_INTERVAL";
const char kHeaderFieldsSvrHost[] = "HEADER_FIELDS_SVR_HOST";
const char kHeaderFieldsSvrPort[] = "HEADER_FIELDS_SVR_PORT";
///add for proxy mode , by wangsj
const char kProxyMode[] =  "PROXY_MODE";
const char kProxyIp[] = "PROXY_IP";


class DownloaderConfig {
 public:
  DownloaderConfig() { }

  int LoadConfig(const std::string &conf_file);

  void PrintConfig() const;

  bool IsDebugMode() { return is_debug_; }
  ///add for proxy mode , by wangsj
  bool IsProxyMode() { return is_proxy_; }
  const std::string GetProxyIp() { return proxy_ip_; }
  ///end
  const std::string & GetDcHost() { return dc_host_; }
  int GetDcPort() { return dc_port_; }
  int GetSocketTimeout() { return socket_timeout_; }
  int GetPersistCount() { return persist_count_; }
  int GetCheckInterval() { return check_interval_; }
  int GetDownloadInterval() { return download_interval_; }
  int GetTimeSlice() { return time_slice_; }
  int GetRetryTimes() { return retry_times_; }
  int GetCacheExpireInterval() { return cache_expire_interval_; }
  int GetDownloadFailLimit() { return download_fail_limit_; }
  int GetDownloadThreadCount() { return download_thread_count_; }
  int GetUploadRetryTimes() { return upload_retry_times_; }
  const std::string &GetLocalInterface() { return local_interface_; }
  int GetCurlOptMaxRedirs() { return curl_opt_max_redirs_; }
  const std::vector<std::string> &GetDomainList() { return domain_list_; }
  bool IsNetCheck() { return is_net_check_; }
  int GetNetCheckTimeout() { return net_check_timeout_; }
  int GetNetCheckInterval() { return net_check_interval_; }
  const std::string HeaderFieldsSvrHost() { return header_fields_svr_host_; }
  int HeaderFieldsSvrPort() { return header_fields_svr_port_; }
  
 private:
  int CheckVal(int val, const std::string &name);

 private:
  bool is_debug_;
  ///add for proxy mode , by wangsj
  bool is_proxy_;
  std::string proxy_ip_;
  ///end
  std::string dc_host_;
  int dc_port_;
  std::string downloader_host_;
  int downloader_port_;
  std::string link_base_host_;
  int link_base_port_;
  int nb_thread_count_;
  int socket_timeout_;
  int persist_count_;
  int check_interval_;
  int download_interval_;       ///< 同一domain下的请求间的下载间隔，单位:秒
  int time_slice_;              ///< dc中，使用时间片控制下载，时间片的长度，单位：毫秒
  int retry_times_;             ///< 重试下载次数的上限
  int cache_expire_interval_;   ///< 下载cache中的过期时间间隔，单位：秒
  int download_fail_limit_;     ///< 转发download请求失败的次数
  int download_thread_count_;   ///< thread count of download
  int upload_retry_times_;      ///< retry times for upload to dc
  std::string local_interface_;      ///< the interface name to use as outgoing network interface
  int curl_opt_max_redirs_;     ///< CURLOPT_MAXREDIRS
  std::vector<std::string> domain_list_;  ///< domain list, for net checker
  bool is_net_check_;           ///< whether to check net
  int net_check_timeout_;       ///< timeout for curl in net check, in millisecs
  int net_check_interval_;      ///< check interval for net check, in millisecs
  std::string header_fields_svr_host_;
  int header_fields_svr_port_;

 private:
  DISALLOW_COPY_AND_ASSIGN(DownloaderConfig);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_CONFIG_H_
