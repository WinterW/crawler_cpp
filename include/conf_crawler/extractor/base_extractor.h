/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/base_extractor.h
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-11
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_BASE_EXTRACTOR_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_BASE_EXTRACTOR_H_

#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>

#include "global.h"
#include "util/thread/thread.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"
#include "util/thread/condition.h"
#include "conf_crawler/extractor/struct_def.h"

#include "conf_crawler/extractor/conf_crawler_types.h"
#include "conf_crawler/extractor/StaticLinkBaseService.h"
#include "conf_crawler/common/long_short_conn.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using ganji::util::thread::Thread;
using ganji::util::thread::Mutex;
using ganji::util::thread::RWLock;
using ganji::util::thread::Condition;

using ganji::crawler::conf_crawler::LongShortConn;

class ExtractorConfig;

/**
 * @class BaseExtractor
 * @brief base extractor
 */
class BaseExtractor {
 public:
  BaseExtractor() {
  }

  virtual ~BaseExtractor() {
  }

  /// @brief Init
  /// @param[in] p_config The extractor config
  /// @return 0:success or -1
  int Init(ExtractorConfig *p_config);

  /// @brief load template file
  /// @param[in] template_name The name of template
  /// @param [out] p_err_info The error info when failed
  /// @return 0:success or -1
  virtual int LoadTemplate(const std::string &template_name, std::string *p_err_info) = 0;

  int GetMultipleTemplates(const std::string &template_path,
                           const std::string &template_prefix,
                           std::vector<std::string> *p_file_list);

  /// @brief push item into extract queue
  /// @param[in] extract_item The extract item
  void PushIntoExtractQueue(const ExtractItem &extract_item);

  /// Extract from Body content, then get the result
  /// @param[in] template_name
  /// @param[in] depth
  /// @param[in] body
  /// @param[out] p_matched_result_item
  /// @return 0:success or -1
  virtual int ExtractWrapper(const ExtractItem &extract_item,
                             MatchedResultItem *p_matched_result_item) = 0;


  /// construct match regex
  /// @param[in] match_pattern The match pattern containing regex
  /// @param[out] p_match_line The match line whose regex to construct
  /// @param[out] p_err_info The error info when failed
  /// @return 0:success -1:failure
  int ConstructMatchRegex(const std::string &match_pattern, BaseMatchLine *p_match_line, std::string *p_err_info);


  /// construct self-defined value
  /// @param[in] conf_str The configure string to construct
  /// @param[out] p_match_line The match line to construct
  /// @param[out] p_err_info The error info when failed
  /// @return 0:success -1:failure
  int ConstructSelfDefinedVal(const string &conf_str, BaseMatchLine *p_match_line, string *p_err_info);

  int UploadExtract(const ExtractItem &extract_item, const MatchedResultItem &matched_result_item);

  /// fill combine field
  /// @param[in] value The combine string with variable to fill
  /// @param[in] self_result The extracted self result
  /// @param[out] p_filled_value The filled combine string
  /// @return 0:success -1:failure
  int FillValue(const std::string &value,
                const SelfResult &self_result,
                std::string *p_filled_value);

  /// Get domain from template name
  /// @param[in] template_name The template name
  /// @param[out] p_domain The domain of the template
  /// @return 0:success -1:failure
  int GetTemplateDomain(const std::string &template_name, std::string *p_domain);
  int ParseTemplateName(const std::string &template_name, std::string *p_domain, std::string *p_category);
 private:
  static void *TimerThread(void *arg);
  int TimerThreadFunc();

  static void *SvrThread(void *arg);
  int SvrThreadFunc();

 public:
  ExtractorConfig *p_config_;

 private:
  LongShortConn<StaticLinkBaseServiceClient> link_base_conn_;     ///< connection with link base

  thread::Thread *p_timer_thread_;  ///< timer thread
  thread::Thread *p_svr_thread_;    ///< server thread

  /// extract request item queue
  std::queue<ExtractItem> extract_queue_;
  Mutex queue_lock_;
  Condition queue_cond_;
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_BASE_EXTRACTOR_H_
