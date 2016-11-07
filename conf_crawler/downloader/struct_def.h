/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/struct_def.h
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_STRUCT_DEF_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_STRUCT_DEF_H_

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

#include "conf_crawler/common/struct_def.h"
#include "conf_crawler/downloader/conf_crawler_types.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
const char kGzip[] = "gzip, deflate";
/// default timeout value for pamameter `CURLOPT_TIMEOUT'
const size_t kDefaultTimeout = 30;

/**
 * @struct HttpReqItem
 * @brief http request item
 */
struct HttpReqItem {
  HttpReqItem()
    : time_out_(kDefaultTimeout) {
  }

  HttpReqItem(const std::string &url,
      const std::string &ip,
      const std::string &referer,
      const std::string &ua,
      const std::string &post_fields,
      size_t time_out,
      HeaderFieldsType::type header_fields_type,
      int depth)
      : url_(url),
    ip_(ip),
    referer_(referer),
    ua_(ua),
    post_fields_(post_fields),
    time_out_(time_out),
    header_fields_type_(header_fields_type),
    depth_(depth) {
  }

  std::string url_;
  std::string ip_;
  std::string referer_;
  std::string ua_;
  std::string post_fields_;
  size_t time_out_;
  HeaderFieldsType::type header_fields_type_;    ///< header fields type
  int depth_;
};

//typedef std::unordered_map<std::string, time_t, StringHash, StringHash> Domain2TimeMap;
typedef std::unordered_map<std::string, uint64_t, StringHash, StringHash> Domain2TimeMap;

/**
 * @struct FriendlyTask
 * @brief serialize request for friendly download sites
 */
struct FriendlyTask {
  FriendlyTask(const DownloadTask &task,
      //time_t hint_time)
      uint64_t hint_time)
    : task_(task),
    hint_time_(hint_time) {
  }

  DownloadTask task_;
  time_t hint_time_;
};

struct CmpDownloadTaskCache: public std::binary_function<FriendlyTask, FriendlyTask, bool> {
  bool operator()(const FriendlyTask &c1, const FriendlyTask &c2) const {
    return c1.hint_time_ > c2.hint_time_;
  }
};
typedef std::priority_queue<FriendlyTask, std::vector<FriendlyTask>, CmpDownloadTaskCache> FriendlyQueue;
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_STRUCT_DEF_H_
