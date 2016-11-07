/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/url_dedup/dedup_config.cc
 * @namespace ganji::crawler::conf_crawler::dedup
 * @version 1.0
 * @author  lisizhong
 * @date    2011-08-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/url_dedup/dedup_config.h"

#include <stdlib.h>

#include "util/config/config.h"

using std::string;

namespace ganji { namespace crawler { namespace conf_crawler { namespace dedup {
using ganji::util::config::Config;

int DedupConfig::CheckVal(int val, const string &name) {
  if (val <= 0) {
    fprintf(stderr, "%s:%d <= 0\n", name.c_str(), val);
    exit(1);
  }
  return 0;
}

int DedupConfig::LoadConfig(const string &conf_file) {
  Config conf;
  conf.loadConfFile(conf_file);
  conf.getItemValue(kDedupHost, dedup_host_);
  conf.getItemValue(kDedupPort, dedup_port_);
  conf.getItemValue(kNbThreadCount, nb_thread_count_);
  conf.getItemValue(kBloomFilterMaxElem, bf_max_elem_);
  string bf_fpf;
  conf.getItemValue(kBloomFilterFPF, bf_fpf);
  bf_fpf_ = atof(bf_fpf.c_str());
  conf.getItemValue(kBucketCount, bucket_count_);
  conf.getItemValue(kDayCount, day_count_);
  conf.getItemValue(kMd5Path, md5_path_);

  CheckVal(dedup_port_, "dedup_port_");
  CheckVal(nb_thread_count_, "nb_thread_count_");
  CheckVal(bf_max_elem_, "bf_max_elem_");
  CheckVal(bucket_count_, "bucket_count_");
  CheckVal(day_count_, "day_count_");

  return 0;
}

void DedupConfig::PrintConfig() const {
  fprintf(stdout, "--------------------------DedupConfig config--------------------------\n");

  fprintf(stdout, "dedup host:%s\n", dedup_host_.c_str());
  fprintf(stdout, "dedup port:%d\n", dedup_port_);
  fprintf(stdout, "nb thread count:%d\n", nb_thread_count_);
  fprintf(stdout, "bloom filter max elem:%d\n", bf_max_elem_);
  fprintf(stdout, "bloom filter fpf:%f\n", bf_fpf_);
  fprintf(stdout, "bucket count:%d\n", bucket_count_);
  fprintf(stdout, "day count:%d\n", day_count_);
  fprintf(stdout, "md5 path:%s\n", md5_path_.c_str());

  fprintf(stdout, "--------------------------DedupConfig config--------------------------\n");
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dedup

