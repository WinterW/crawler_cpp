/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/dev_link_base.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_DEV_LINK_BASE_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_DEV_LINK_BASE_H_

#include <string>
#include <vector>
#include <map>
#include <set>

#include "ganji/ganji_global.h"
#include "ganji/util/thread/thread.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"
#include "ganji/util/thread/condition.h"

#include "ganji/crawler/conf_crawler/common/long_conn.h"
#include "ganji/crawler/conf_crawler/link_base/struct_def.h"
#include "ganji/crawler/conf_crawler/link_base/mongo_storage.h"

#include "ganji/crawler/conf_crawler/dc/conf_crawler_types.h"
#include "ganji/crawler/conf_crawler/link_base/DCService.h"
#include "ganji/crawler/conf_crawler/link_base/ExtractorService.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
using std::string;
using std::vector;
using std::map;
using std::set;

class LinkConfig;

namespace thread = ganji::util::thread;
using thread::Thread;
using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::util::thread::Condition;

using ganji::crawler::conf_crawler::LongConnHandler;

/**
 * @class DevLinkBase
 * @brief 管理链接，整个爬虫的控制中心
 */
class DevLinkBase {
 public:
  DevLinkBase()
    : p_config_(NULL) {
  }

  ~DevLinkBase() {
  }

  int Init(LinkConfig *p_config);

  /// @brief 处理下载和抽取请求
  int DevUrl(const DevUrlItem &dev_url_item, MatchedResultItem *matched_result_item);

  int ExtractUpdateConf();

 private:
  /// @brief 同步下载
  /// @return 下载成功当且仅当返回值为0且downloaded_body_item->is_ok 
  int DownloadSync(const DevUrlItem &dev_url_item, DownloadedBodyItem *downloaded_body_item);

  /// @brief 同步抽取
  /// @return 抽取成功当且仅当返回值为0且matched_result_item->is_ok 
  int ExtractSync(const DevUrlItem &dev_url_item,
                  const string &body,
                  MatchedResultItem *matched_result_item);

 private:
  LinkConfig *p_config_; 

  LongConnHandler<DCServiceClient> dc_conn_;            ///< 与dc的长连接
  LongConnHandler<ExtractorServiceClient> extractor_conn_;      ///< 与extractor的长连接

  MongoStorage mongo_;      ///< mongo存储器

  char err_buf_[kErrBufLen];

 private:
  DISALLOW_COPY_AND_ASSIGN(DevLinkBase);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_DEV_LINK_BASE_H_
