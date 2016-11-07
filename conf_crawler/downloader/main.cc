/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/main.cc
 * @namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-26
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <string>

#include "util/net/http_opt.h"
#include "util/log/thread_fast_log.h"
#include "conf_crawler/downloader/downloader.h"
#include "conf_crawler/downloader/downloader_config.h"
#include "conf_crawler/downloader/curl_downloader.h"
#include "conf_crawler/downloader/struct_def.h"

using std::string;

using ganji::crawler::conf_crawler::downloader::Downloader;
using ganji::crawler::conf_crawler::downloader::DownloaderConfig;
using ganji::crawler::conf_crawler::downloader::CurlDownloader;
using ganji::crawler::conf_crawler::downloader::CurlGlobal;
using ganji::crawler::conf_crawler::downloader::HttpReqItem;

using namespace ganji::util::log::ThreadFastLog;
namespace FastLog = ganji::util::log::ThreadFastLog;
namespace Http = ::ganji::util::net::Http;

void Test(DownloaderConfig *config, const string &url, const string &ip) {
  CurlDownloader curl_downloader;
  curl_downloader.Init(config);

  string ua = "Mozilla";
  size_t to = 30;

  HttpReqItem req_item;
  req_item.url_ = url;
  req_item.ip_ = ip;
  req_item.ua_ = ua;
  req_item.time_out_ = to;

  string body;
  curl_downloader.Perform(req_item, &body);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:%s conf_path log_path\n", argv[0]);
    return 1;
  }

  string conf_path = argv[1];
  string log_path = argv[2];


  CurlGlobal curl_global;

  static FastLogStat log_st = {kLogAll, kLogNone, kLogSizeSplit};
  FastLog::OpenLog(log_path.c_str(), "downloader", 2048, &log_st);

  DownloaderConfig config;
  config.LoadConfig(conf_path);
  config.PrintConfig();

#if 0
  if (argc < 3) {
    printf("Usage:%s url ip\n", argv[0]);
    return 1;
  }
  string url = argv[1];
  string ip = argv[2];
  Test(&config, url, ip);
  return 0;
#endif

  Downloader downloader;
  int ret = downloader.Init(&config);
  if (ret < 0) {
    WriteLog(kLogFatal, "Downloader Init failed");
    return 1;
  }

  FastLog::WriteLog(kLogNotice, "--------------Downloader Starting!-------------");

  downloader.Run();

  FastLog::WriteLog(kLogNotice, "--------------Downloader Stopped!-------------");
  FastLog::CloseLog(0);

  return 0;
}

