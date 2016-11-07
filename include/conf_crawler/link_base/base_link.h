/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/base_link.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-19
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE_LINK_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE_LINK_H_

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <list>

#include "ganji/ganji_global.h"
#include "ganji/util/thread/thread.h"
#include "ganji/util/thread/mutex.h"
#include "ganji/util/thread/rwlock.h"
#include "ganji/util/thread/condition.h"

#include "ganji/crawler/conf_crawler/common/long_short_conn.h"
#include "ganji/crawler/conf_crawler/link_base/struct_def.h"
#include "ganji/crawler/conf_crawler/link_base/mongo_storage.h"
#include "ganji/crawler/conf_crawler/link_base/base64.h"

#include "ganji/crawler/conf_crawler/link_base/DedupService.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
class LinkConfig;

using ganji::util::thread::Thread;
using ganji::util::thread::RWLock;
using ganji::util::thread::Mutex;
using ganji::util::thread::Condition;

using ganji::crawler::conf_crawler::LongShortConn;

/**
 * @class BaseLink
 * base class of manage link, controller of crawler system
 */
class BaseLink {
 public:
  BaseLink()
    : p_config_(NULL),
    p_timer_thread_(NULL),
    p_upload_body_thread_(NULL),
    p_upload_extract_thread_(NULL) {
  }

  virtual ~BaseLink() {
    Thread::FreeThread(p_timer_thread_);
    Thread::FreeThread(p_upload_body_thread_);
    Thread::FreeThread(p_upload_extract_thread_);
  }

  /// Init
  /// @param[in] p_config LinkConfig
  /// @return 0:success -1:failure
  int Init(LinkConfig *p_config);

  /// dc pull task
  /// @param[out] p_list download task list
  /// @return 0:success -1:failure
  int OnGetDownloadTask(std::vector<DownloadTask> *p_list);

  /// extractor pull task
  /// @param[out] p_list extract task list
  /// @return 0:success -1:failure
  int OnGetExtractItem(std::vector<ExtractItem> *p_list);

  /// process body uploaded by dc
  /// @paramp[in] downloaded_body_item The downloaded body item uploaded by dc
  void OnUploadBody(const DownloadedBodyItem &downloaded_body_item);

  /// process extract result uploaded by extractor
  /// @paramp[in] extract_item The extract item
  /// @paramp[in] matched_result_item The matched result
  void OnUploadExtract(const ExtractItem &extract_item, const MatchedResultItem &matched_result_item);

 private:
  /// fill download task by seed, called by OnGetDownloadTask()
  /// @param[in] seed_url The seed url
  /// @param[in] depth The depth
  /// @param[out] p_task The download task to fill
  /// @return 0:success -1:failure
  virtual int FillDownloadTaskBySeed(const std::string &seed_url, int depth, DownloadTask *p_task) = 0;

  /// Timer thread function
  static void * TimerThread(void *arg);
  virtual void TimerThreadFunc() = 0;

  /// process uploaded body by dc
  static void * UploadBodyThread(void *arg);
  int UploadBodyThreadFunc();

  /// Process downloaded body by dc, called by UploadBodyThreadFunc()
  /// @param[in] downloaded_body_item The downloaded body item
  virtual void ProcUploadBody(const DownloadedBodyItem &downloaded_body_item) = 0;

  /// process uploaded extract result by extractor
  static void * UploadExtractThread(void *arg);
  int UploadExtractThreadFunc();

  /// Process extract item uploaded by Extractor, called by UploadExtractThreadFunc()
  /// @param[in] extract_result_item The extract result item
  /// @return 0:success -1:failure
  virtual int ProcUploadExtract(const ExtractResultItem &extract_result_item) = 0;

 public:
  /// expand link map
  void ExpandLinkByMatchedResult(const ExtractItem &extract_item,
                                 const MatchedResultItem &matched_result_item,
                                 std::list<ExpandLinkItem> *p_next_link_item_list,
                                 std::list<ExpandLinkItem> *p_next_page_item_list);

  /// expand link map
  void ExpandLinkByMap(const ExtractItem &extract_item,
                       const std::map<std::string, std::vector<std::string> > &result_map,
                       std::list<ExpandLinkItem> *p_next_link_item_list,
                       std::list<ExpandLinkItem> *p_next_page_item_list);

  /// generete link
  void GenerateLink(const std::vector<std::string> &link_list,
                    const std::string &seed_url,
                    const std::string &referer,
                    bool is_img_url,
                    bool is_phone_img_url,
                    int depth,
                    std::list<ExpandLinkItem> *p_next_link_item_list);

  /// dedup link of depth>=1
  int Dedup(int max_depth, const std::list<ExpandLinkItem> &next_link_item_list, int *p_new_link_count);

  /// check whether url is in deduplication server
  int TestExistsAndInsert(const string &url, bool *p_exists);

  /// Whether url is absolute url
  bool IsAbsUrl(const string &url);

  /// Get abs url from relative form
  int Relative2AbsUrl(const string &referer, const string &rel_url, string *abs_url);

  /// Parse extract type into list
  int ParseTemplateTypeList(const std::string &template_type, SeedItem *p_seed);

  int ParseTemplateType(const std::string &template_type, TemplateType::type *p_ex_type);

  bool IsBanned(const std::map<std::string, std::vector<std::string> > &result_map);

 public:
  LinkConfig *p_config_;

  LongShortConn<DedupServiceClient> dedup_conn_;      ///< connection with dedup svr

  Base64 base64_;           ///< base64 coder
  MongoBody mongo_body_;      ///< mongo storage for body_db
  MongoExtract mongo_extract_;      ///< mongo storage for extract_db
  MongoBody mongo_img_;      ///< mongo storage for img_db
  MongoSeed mongo_seed_;      ///< load from mongo seed_db

  /// lock for seed map
  RWLock seed_item_lock_;

  /// download request queue
  std::queue<ExpandLinkItem> expand_link_queue_;
  Mutex link_queue_lock_;
  Condition link_queue_cond_;

  /// download request queue
  std::queue<ExpandLinkItem> delay_expand_link_queue_;
  Mutex delay_link_queue_lock_;

  /// download result queue
  std::queue<DownloadedBodyItem> downloaded_queue_;
  Mutex downloaded_lock_;
  Condition downloaded_cond_;

  /// extract request queue
  std::queue<ExtractItem> extract_req_queue_;
  Mutex extract_req_lock_;
  Condition extract_req_cond_;

  /// extract result queue
  std::queue<ExtractResultItem> extracted_queue_;
  Mutex extracted_lock_;
  Condition extracted_cond_;

  Thread *p_timer_thread_;
  Thread *p_upload_body_thread_;
  Thread *p_upload_extract_thread_;

 private:
  DISALLOW_COPY_AND_ASSIGN(BaseLink);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE_LINK_H_
