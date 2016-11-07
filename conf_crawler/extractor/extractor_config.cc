/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/extractor/extractor_config.cc
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-25
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/extractor/extractor_config.h"

#include <stdlib.h>

#include "util/config/config.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using std::string;
using ganji::util::config::Config;

int ExtractorConfig::CheckVal(int val, const string &name) {
  if (val <= 0) {
    fprintf(stderr, "%s:%d <= 0\n", name.c_str(), val);
    exit(1);
  }
  return 0;
}


int ExtractorConfig::LoadConfig(const string &conf_file) {
  Config conf;
  conf.loadConfFile(conf_file);
  conf.getItemValue(kNbThreadCount, nb_thread_count_);
  conf.getItemValue(kSocketTimeout, socket_timeout_);
  conf.getItemValue(kPersistCount, persist_count_);
  conf.getItemValue(kSvrThreadCount, svr_thread_count_);
  conf.getItemValue(kExtractorPort, extractor_port_);
  conf.getItemValue(kLinkBaseHost, link_base_host_);
  conf.getItemValue(kLinkBasePort, link_base_port_);
  conf.getItemValue(kTimeSlice, time_slice_);
  conf.getItemValue(kReqRetryTimes, req_retry_times_);
  conf.getItemValue(kRelaySvrHost, relay_svr_host_);
  conf.getItemValue(kRelaySvrPort, relay_svr_port_);
  conf.getItemValue(kTemplatePath, template_path_);
  int is_get_task = 0;
  conf.getItemValue(kIsGetTask, is_get_task);
  if (is_get_task == 1)
    is_get_task_ = true;
  else
    is_get_task_ = false;

  CheckVal(nb_thread_count_, "nb_thread_count_");
  CheckVal(socket_timeout_, "socket_timeout_");
  CheckVal(persist_count_, "persist_count_");
  CheckVal(svr_thread_count_, "svr_thread_count_");
  CheckVal(extractor_port_, "extractor_port_");
  CheckVal(link_base_port_, "link_base_port_");
  CheckVal(time_slice_, "time_slice_");
  CheckVal(req_retry_times_, "req_retry_times_");
  CheckVal(relay_svr_port_, "relay_svr_port_");

  return 0;
}

void ExtractorConfig::PrintConfig() const {
  fprintf(stdout, "--------------------------ExtractorConfig config--------------------------\n");

  fprintf(stdout, "nb thread count:%d\n", nb_thread_count_);
  fprintf(stdout, "socket timeout:%d\n", socket_timeout_);
  fprintf(stdout, "persist count:%d\n", persist_count_);
  fprintf(stdout, "svr thread count:%d\n", svr_thread_count_);
  fprintf(stdout, "extractor port:%d\n", extractor_port_);
  fprintf(stdout, "link base host:%s\n", link_base_host_.c_str());
  fprintf(stdout, "link base port:%d\n", link_base_port_);
  fprintf(stdout, "time slice:%d\n", time_slice_);
  fprintf(stdout, "req retry times:%d\n", req_retry_times_);
  fprintf(stdout, "relay svr port:%d\n", relay_svr_port_);
  fprintf(stdout, "template path:%s\n", template_path_.c_str());
  fprintf(stdout, "is_get_task_:%d\n", is_get_task_);

  fprintf(stdout, "--------------------------ExtractorConfig config--------------------------\n");
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

