/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/dc/dc_config.cc
 * @namespace ganji::crawler::conf_crawler::dc
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "dc_config.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util/config/config.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dc {
using std::string;
using std::vector;
using ganji::util::config::Config;

int DcConfig::CheckVal(int val, const string &name) {
  if (val <= 0) {
    fprintf(stderr, "%s:%d <= 0\n", name.c_str(), val);
    exit(1);
  }
  return 0;
}


int DcConfig::LoadConfig(const string &conf_file) {
  Config conf;
  conf.loadConfFile(conf_file);
  conf.getItemValue(kDcHost, dc_host_);
  conf.getItemValue(kDcPort, dc_port_);
  conf.getItemValue(kLinkBaseHost, link_base_host_);
  conf.getItemValue(kLinkBasePort, link_base_port_);
  conf.getItemValue(kNbThreadCount, nb_thread_count_);
  conf.getItemValue(kSocketTimeout, socket_timeout_);
  conf.getItemValue(kPersistCount, persist_count_);
  conf.getItemValue(kCheckInterval, check_interval_);
  conf.getItemValue(kDownloadInterval, download_interval_);
  conf.getItemValue(kTimeSlice, time_slice_);
  conf.getItemValue(kRetryTimes, retry_times_);
  conf.getItemValue(kCacheExpireInterval, cache_expire_interval_);
  conf.getItemValue(kDownloadTimeout, download_time_out_);
  conf.getItemValue(kGbkUtf16File, gbk_utf16_file_);
  conf.getItemValue(kGetTaskThreadCount, get_task_thread_count_);
  conf.getItemValue(kUserAgentFile, ua_file_);
  conf.getItemValue(kFailedDelayTime,failed_delay_time_);

  if (failed_delay_time_ <= 0)
    failed_delay_time_ = 120;

  /// load ua file
  if (LoadUaFile() < 0) {
    return -1;
  }

  CheckVal(dc_port_, "dc_port_");
  CheckVal(link_base_port_, "link_base_port_");
  CheckVal(nb_thread_count_, "nb_thread_count_");
  CheckVal(socket_timeout_, "socket_timeout_");
  CheckVal(persist_count_, "persist_count_");
  CheckVal(check_interval_, "check_interval_");
  CheckVal(download_interval_, "download_interval_");
  CheckVal(time_slice_, "time_slice_");
  CheckVal(retry_times_, "retry_times_");
  CheckVal(cache_expire_interval_, "cache_expire_interval_");
  CheckVal(download_time_out_, "download_time_out_");
  CheckVal(get_task_thread_count_, "get_task_thread_count_");

  return 0;
}

void DcConfig::PrintConfig() const {
  fprintf(stdout, "--------------------------DcConfig config--------------------------\n");

  fprintf(stdout, "dc host:%s\n", dc_host_.c_str());
  fprintf(stdout, "dc port:%d\n", dc_port_);
  fprintf(stdout, "link base host:%s\n", link_base_host_.c_str());
  fprintf(stdout, "link base port:%d\n", link_base_port_);
  fprintf(stdout, "nb thread count:%d\n", nb_thread_count_);
  fprintf(stdout, "socket timeout:%dsec\n", socket_timeout_);
  fprintf(stdout, "persist count:%d\n", persist_count_);
  fprintf(stdout, "check interval:%d\n", check_interval_);
  fprintf(stdout, "download interval:%dsec\n", download_interval_);
  fprintf(stdout, "time slice:%dmillisecond\n", time_slice_);
  fprintf(stdout, "retry times:%d\n", retry_times_);
  fprintf(stdout, "cache expire interval:%dsec\n", cache_expire_interval_);
  fprintf(stdout, "download timeout:%d\n", download_time_out_);
  fprintf(stdout, "gbk utf16 file:%s\n", gbk_utf16_file_.c_str());
  fprintf(stdout, "get task thread count:%d\n", get_task_thread_count_);
  fprintf(stdout, "user agent file:%s\n", ua_file_.c_str());
  fprintf(stdout, "failed delay time:%d\n", failed_delay_time_);

  fprintf(stdout, "--------------------------DcConfig config--------------------------\n");
}

void DcConfig::GetUserAgent(string *p_ua) {
  p_ua->clear();
  if (!ua_list_.empty()) {
    *p_ua = ua_list_[random() % ua_list_.size()];
  }
}

int DcConfig::LoadUaFile() {
  FILE *fp = fopen(ua_file_.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "open:%s failed:%s",
            ua_file_.c_str(),
            strerror(errno));
    return -1;
  }

  size_t n = 0;
  char *p_line = NULL;
  while (getline(&p_line, &n, fp) != -1) {
    if (strlen(p_line) <= 1)
      continue;
    p_line[strlen(p_line)-1] = '\0';
    ua_list_.push_back(p_line);
  }

  if (p_line)
    free(p_line);
  fclose(fp);

  return 0;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dc

