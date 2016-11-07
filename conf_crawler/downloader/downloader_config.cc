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

#include "downloader_config.h"

#include <stdlib.h>

#include "util/text/text.h"
#include "util/config/config.h"
#include "util/log/thread_fast_log.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
using std::string;
using std::vector;
using ganji::util::config::Config;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
namespace Text = ::ganji::util::text::Text;

int DownloaderConfig::CheckVal(int val, const string &name) {
  if (val <= 0) {
    fprintf(stderr, "%s:%d <= 0\n", name.c_str(), val);
    exit(1);
  }
  return 0;
}

int DownloaderConfig::LoadConfig(const string &conf_file) {
  Config conf;
  conf.loadConfFile(conf_file);
  string debug_mode;
  conf.getItemValue(kDebugMode, debug_mode);
  if (debug_mode == "1") 
    is_debug_ = true;
  else
    is_debug_ = false;
  ///add for proxy mode , by wangsj
  string proxy_mode;
  conf.getItemValue(kProxyMode, proxy_mode);
  if(proxy_mode == "1")
    is_proxy_ = true;
  else
    is_proxy_ = false;
  conf.getItemValue(kProxyIp, proxy_ip_);
  //end
  conf.getItemValue(kDcHost, dc_host_);
  conf.getItemValue(kDcPort, dc_port_);
  conf.getItemValue(kSocketTimeout, socket_timeout_);
  conf.getItemValue(kPersistCount, persist_count_);
  conf.getItemValue(kCheckInterval, check_interval_);
  conf.getItemValue(kDownloadInterval, download_interval_);
  conf.getItemValue(kTimeSlice, time_slice_);
  conf.getItemValue(kRetryTimes, retry_times_);
  conf.getItemValue(kCacheExpireInterval, cache_expire_interval_);
  conf.getItemValue(kDownloadFailLimit, download_fail_limit_);
  conf.getItemValue(kDownloadThreadCount, download_thread_count_);
  conf.getItemValue(kUploadRetryTimes, upload_retry_times_);
  conf.getItemValue(kLocalInterface, local_interface_);
  conf.getItemValue(kCurlOptMaxRedirs, curl_opt_max_redirs_);
  string domain_str;
  conf.getItemValue(kDomainList, domain_str);
  int is_net_check;
  conf.getItemValue(kIsNetCheck, is_net_check);
  if (is_net_check == 1)
    is_net_check_ = true;
  else
    is_net_check_ = false;
  conf.getItemValue(kNetCheckTimeout, net_check_timeout_);
  conf.getItemValue(kNetCheckInterval, net_check_interval_);
  conf.getItemValue(kHeaderFieldsSvrHost, header_fields_svr_host_);
  conf.getItemValue(kHeaderFieldsSvrPort, header_fields_svr_port_);
  
  CheckVal(dc_port_, "dc_port_");
  CheckVal(socket_timeout_, "socket_timeout_");
  CheckVal(persist_count_, "persist_count_");
  CheckVal(check_interval_, "check_interval_");
  CheckVal(download_interval_, "download_interval_");
  CheckVal(time_slice_, "time_slice_");
  CheckVal(retry_times_, "retry_times_");
  CheckVal(cache_expire_interval_, "cache_expire_interval_");
  CheckVal(download_fail_limit_, "download_fail_limit_");
  CheckVal(download_thread_count_, "download_thread_count_");
  CheckVal(upload_retry_times_, "upload_retry_times_");
  CheckVal(curl_opt_max_redirs_, "curl_opt_max_redirs_");
  CheckVal(net_check_timeout_, "net_check_timeout_");
  CheckVal(net_check_interval_, "net_check_interval_");
  CheckVal(header_fields_svr_port_, "header_fields_svr_port_");

  Text::Segment(domain_str, kDelim, &domain_list_);
  if (domain_list_.empty()) {
    WriteLog(kLogFatal, "domain list empty");
    return -1;
  }

  return 0;
}

void DownloaderConfig::PrintConfig() const {
  fprintf(stdout, "--------------------------DownloaderConfig config--------------------------\n");

  fprintf(stdout, "is debug mode:%d\n", is_debug_);
  fprintf(stdout, "dc host:%s\n", dc_host_.c_str());
  fprintf(stdout, "dc port:%d\n", dc_port_);
  fprintf(stdout, "socket timeout:%dsec\n", socket_timeout_);
  fprintf(stdout, "persist count:%d\n", persist_count_);
  fprintf(stdout, "check interval:%d\n", check_interval_);
  fprintf(stdout, "download interval:%dsec\n", download_interval_);
  fprintf(stdout, "time slice:%dmillisecond\n", time_slice_);
  fprintf(stdout, "retry times:%d\n", retry_times_);
  fprintf(stdout, "cache expire interval:%dsec\n", cache_expire_interval_);
  fprintf(stdout, "download fail limit:%d\n", download_fail_limit_);
  fprintf(stdout, "download thread count:%d\n", download_thread_count_);
  fprintf(stdout, "upload retry times:%d\n", upload_retry_times_);
  fprintf(stdout, "local interface:%s\n", local_interface_.c_str());
  fprintf(stdout, "curl opt max redirs:%d\n", curl_opt_max_redirs_);
  fprintf(stdout, "is net check:%d\n", is_net_check_);
  fprintf(stdout, "net check timeout:%dms\n", net_check_timeout_);
  fprintf(stdout, "net check interval:%dms\n", net_check_interval_);
  fprintf(stdout, "header_fields_svr_host_:%s\n", header_fields_svr_host_.c_str());
  fprintf(stdout, "header_fields_svr_port_:%d\n", header_fields_svr_port_);
  fprintf(stdout, "proxy_ip_:%s\n", proxy_ip_.c_str());
  
  fprintf(stdout, "--------------------------DownloaderConfig config--------------------------\n");
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

