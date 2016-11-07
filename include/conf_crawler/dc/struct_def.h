/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/dc/struct_def.h
 * @namespace ganji::crawler::conf_crawler
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DC_STRUCT_DEF_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DC_STRUCT_DEF_H_

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <queue>

#include "conf_crawler/common/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dc {
const char kGzip[] = "gzip, deflate";
/**
 * @struct DownloadTaskItem
 * @brief download task item
 */
struct DownloadTaskItem {
  DownloadTaskItem()
    : count_(0),
    start_time_(0) {
  }

  DownloadReqItem req_item_;
  DownloadPropItem prop_item_;
  int count_;                     ///< downloaded times
  time_t start_time_;             ///< timestamp the task taken by downloader
};

typedef std::queue<DownloadTaskItem> DownloadTaskQueue;
typedef std::unordered_map<std::string, DownloadTaskItem, StringHash, StringHash> DownloadTaskMap;

/// dns cache map
typedef std::unordered_map<std::string, std::string, StringHash, StringHash> DnsCacheMap;
}}}};

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DC_STRUCT_DEF_H_
