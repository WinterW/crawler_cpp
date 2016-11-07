/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_base.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_BASE_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_BASE_H_

#include "ganji/crawler/conf_crawler/link_base/base_link.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
/**
 * @class LinkBase
 * @brief manage link, controller of crawler system
 */
class LinkBase: public BaseLink {
 public:
  LinkBase() {
  }

  ~LinkBase() {
  }

  /// @brief load seed from db by id
  /// @param[in] id The id in seed_table
  /// @param[in] is_add_link Whether to add link
  /// @return 0:success -1:failure
  int LoadSeedById(int id, bool is_add_link);

  /// @brief load seed from db by seed_url
  /// @param[in] seed_url The url of the seed
  /// @param[in] is_add_link Whether to add link
  /// @return 0:success -1:failure
  int LoadSeedByUrl(const std::string &seed_url, bool is_add_link);

 private:
  /// @brief load seed from db by sql
  /// @param[in] select_sql The select sql
  /// @param[in] is_add_link Whether to add link
  /// @param[out] p_erase_list The erased seed url
  /// @param[out] p_update_list The updated seed url
  /// @return 0:success -1:failure
  int LoadSeedFromDb(const std::string &sql, bool is_add_link, std::list<std::string> *p_erase_list, std::list<std::string> *p_update_list);

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
  /// Get seed link to crawl from seed map
  void RefreshSeeds();
  void RefreshDelayTask();

  /// Whether it is night time
  bool IsNight();

  /// Adjust seed frequency
  /// @param[in] seed_url The seed url whose frequency to adjust
  /// @param[in] new_link_count The count of new link get by the current crawl
  void AdjustSeedFreq(const std::string &seed_url, int new_link_count);
  
  /// Process seeds when it is the day night transition point
  void ProcessNightDayPoint();

  /// Save seed info into db, e.g. freq
  /// @return 0:success -1:failure
  int SaveSeedInfoIntoDb();

 private:
  /// seed map
  SeedItemMap seed_item_map_;

 private:
  DISALLOW_COPY_AND_ASSIGN(LinkBase);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_BASE_H_
