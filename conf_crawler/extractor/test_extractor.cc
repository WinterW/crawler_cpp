/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/conf_extractor/main.cc
 * @namespace main func
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <string>
#include <vector>
#include <map>

#include "ganji/crawler/conf_crawler/extractor/css_extractor.h"
#include "ganji/crawler/conf_crawler/extractor/struct_def.h"
// #include "ganji/crawler/conf_extractor/extrator_config.h"
#include "ganji/util/log/thread_fast_log.h"

using std::string;
using std::vector;
using std::map;

using ganji::crawler::conf_crawler::extractor::CssExtractor;
//using ganji::crawler::conf_extractor::ExtractorConfig;

using namespace ganji::util::log::ThreadFastLog;
namespace FastLog = ganji::util::log::ThreadFastLog;

void ShowMap(const map<string, vector<string> > &self_result) {
  for (map<string, vector<string> >::const_iterator it = self_result.begin();
      it != self_result.end(); ++it) {
    printf("%s:%s\n", it->first.c_str(), it->second[0].c_str());
  }
}


int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage:%s template_file depth html_file\n", argv[0]);
    return 1;
  }

  string template_name = argv[1];
  int depth = atoi(argv[2]);
  string html_file = argv[3];

  static FastLogStat log_st = {kLogAll, kLogFatal, kLogSizeSplit};
  FastLog::OpenLog("log", "test", 2048, &log_st);

  string file_in = html_file;
  FILE *fp = fopen(file_in.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "open %s failed:%s\n",
            file_in.c_str(), strerror(errno));
    return 1;
  }
  string html;
  const int kBufSize = 1024;
  char buf[kBufSize];
  while (fgets(buf, kBufSize, fp) != NULL) {
    html += string(buf);
  }
  fclose(fp);

  CssExtractor conf_extractor;
  string err_info;
  int ret = conf_extractor.LoadTemplate(template_name, &err_info);
  if (ret < 0) {
    fprintf(stderr, "LoadTemplate:%s failed\n", template_name.c_str());
    return 1;
  }

  ExtractItem extract_item;
  extract_item.url_template = template_name;
  extract_item.depth = depth;
  extract_item.body = html;
  MatchedResultItem matched_result_item;
  ret = conf_extractor.ExtractWrapper(extract_item, &matched_result_item);
  if (ret < 0) {
    fprintf(stderr, "match failed\n");
    return 1;
  }

  const map<string, vector<string> > &self_result = matched_result_item.self_result;
  const vector<map<string, vector<string> > > &sub_result_list = matched_result_item.sub_result_list;
  printf("self_result:%lu\n", self_result.size());
  ShowMap(self_result);
  printf("sub_result_list:%lu\n", sub_result_list.size());
  for (vector<map<string, vector<string> > >::const_iterator it = sub_result_list.begin();
      it != sub_result_list.end(); ++it) {
    const map<string, vector<string> > &m = *it;
    ShowMap(m);
  }

  return 0;
}

