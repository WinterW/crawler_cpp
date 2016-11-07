/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/static_link_base.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_STATIC_LINK_BASE_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_STATIC_LINK_BASE_H_

#include "ganji/crawler/conf_crawler/link_base/base_link.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
/**
 * @class StaticLinkBase
 * @brief manage link, controller of crawler system
 */
class StaticLinkBase: public BaseLink {
 public:
  StaticLinkBase() {
  }

  ~StaticLinkBase() {
  }

  int LoadFromDbById(int id) {
    return 0;
  }

  /// @brief load seed from db by task_id
  /// @param[in] task_id task id
  /// @param[in] is_add_task whether to add seed as new task
  /// @return 0:success -1:failure
  int LoadDbTask(int task_id, bool is_add_task);

  /// @brief load seed from mongodb by task_id
  /// @param[in] task_id task id
  /// @param[in] is_add_task whether to add seed as new task
  /// @return 0:success -1:failure
  int LoadMongoDbTask(int task_id, bool is_add_task);

 private:
  /// fill download task by seed
  /// @param[in] seed_url The seed url
  /// @param[in] depth The depth
  /// @param[out] p_task The download task to fill
  /// @return 0:success -1:failure
  int FillDownloadTaskBySeed(const std::string &seed_url, int depth, DownloadTask *p_task);

  /// Process downloaded body by dc, called by UploadBodyThreadFunc()
  /// @param[in] downloaded_body_item The downloaded body item
  void ProcUploadBody(const DownloadedBodyItem &downloaded_body_item);

  /// Process extract item uploaded by Extractor, called by UploadExtractThreadFunc()
  /// @param[in] extract_result_item The extract result item
  /// @return 0:success -1:failure
  int ProcUploadExtract(const ExtractResultItem &extract_result_item);

  /// Timer thread function
  void TimerThreadFunc();

 private:
  /// seed map
  StaticSeedItemMap seed_item_map_;
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_STATIC_LINK_BASE_H_
