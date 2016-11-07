/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/css_extractor.h
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-09
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_CSS_EXTRACTOR_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_CSS_EXTRACTOR_H_

#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>

#include <hcxselect.h>

#include "global.h"
#include "util/thread/thread.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"
#include "util/thread/condition.h"
#include "conf_crawler/extractor/struct_def.h"
#include "conf_crawler/extractor/base_extractor.h"

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
using hcxselect::Selector;

/**
 * @class CssExtractor
 * @brief extract by css selector
 */
class CssExtractor: public BaseExtractor {
 public:
  CssExtractor() {
  }

  ~CssExtractor() {
  }

  /// @brief load template file
  /// @param[in] template_name The name of template
  /// @param [out] p_err_info The error info when failed
  /// @return 0:success or -1
  int LoadTemplate(const std::string &template_name, std::string *p_err_info);

  /// Extract from Body content, then get the result
  /// @param[in] template_name
  /// @param[in] depth
  /// @param[in] body
  /// @param[out] p_matched_result_item
  /// @return 0:success or -1
  int ExtractWrapper(const ExtractItem &extract_item,
                     MatchedResultItem *p_matched_result_item);

  /// @brief clear
  void Clear(const std::string &url_template) {
    template_lock_.WrLock();
    template_map_.erase(url_template);
    template_combine_map_.erase(url_template);
    template_lock_.Unlock();
  }

 private:
  /// load config file
  /// @param[in] template_name Template name
  /// @param[in] template_file Path for template file
  /// @param [out] p_depth_template_map The template map for template_name
  /// @param [out] p_depth_combine_map The combine map for template_name
  /// @param [out] p_err_info The error info when failed  
  /// @return 0:success -1:not
  int LoadTemplate(const std::string &template_name,
                   const std::string &template_file,
                   DepthCssTemplateMap *p_depth_template_map,
                   DepthTemplateCombineMap *p_depth_combine_map,
                   std::string *p_err_info);

  /// Extract from Body content, then get the result
  /// @param[in] template_name The template name
  /// @param[in] depth The depth
  /// @param[in] body The html body
  /// @param[out] p_matched_result_item The matched result
  /// @return 0:success or -1
  int Extract(const ExtractItem &extract_item,
              MatchedResultItem *p_matched_result_item);

  int Extract(const std::string &full_template_name,
              const DepthCssTemplateMap &depth_template_map,
              const DepthTemplateCombineMap &depth_combine_map,
              const ExtractItem &extract_item,
              MatchedResultItem *p_matched_result_item);

  /// Extract by one match line
  /// @param[in] full_template_name The full name of template
  /// @param[in] extract_item The extract item
  /// @param[in] body The body with html entities decoded
  /// @param[in] match_line The match line
  /// @param[in] root The root node to start scanning
  /// @param[out] p_matched_result_item The matched result
  /// @return 0:success or -1
  int ExtractByMatchLine(const std::string &full_template_name,
                         const ExtractItem &extract_item,
                         const std::string &body,
                         const CssMatchLine &match_line,
                         Selector &root,
                         MatchedResultItem *p_matched_result_item);

  /// get nodes by css selector
  /// @param[in] selector_str The css selector
  /// @param[in] root The root node to start scanning
  /// @param[out] p_s The selected nodes
  /// @return 0:success -1:failure
  int GetNodesBySelector(const std::string &selector_str, Selector &root, Selector *p_s);

  /// get attribute from nodes
  /// @param[in] s The nodes to select from
  /// @param[in] match_attr The attribute name to match
  /// @param[out] p_list The extracted attribute value list
  /// @return 0:success -1:failure
  int GetNodesAttr(const Selector &s, const std::string &match_attr, std::list<std::string> *p_list);

  /// match line with regex
  /// @param[in] match_line The match line
  /// @param[in] body
  /// @param[in] pos, start position
  /// @param[out] result_map
  /// @param[out] next_pos
  /// @return 0:success -1:failure
  int MatchLineWithRegex(const CssMatchLine &match_line,
                         const std::string &body,
                         SubResultList *p_sub_result_vec);

 private:
  CssTemplateMap template_map_;
  CssTemplateNameMap template_name_map_;
  TemplateCombineListMap template_combine_map_;
  RWLock template_lock_;                                          ///< protect previous data structures
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_PLAIN_EXTRACTOR_H_
