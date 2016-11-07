/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/plain_extractor.h
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lvwei(original author)
 * @author  lisizhong(transformed by)
 * @date    2011-07-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_PLAIN_EXTRACTOR_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_PLAIN_EXTRACTOR_H_

#include <string>
#include <vector>
#include <queue>
#include <map>

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

/**
 * @class PlainExtractor
 * @brief maitain template to extract
 */
class PlainExtractor: public BaseExtractor {
 public:
  PlainExtractor() {
  }

  virtual ~PlainExtractor() {
  }

  /// @brief load template file
  int LoadTemplate(const std::string &template_name, std::string *p_err_info);

  /// Extract from Body content, then get the result
  /// @param [in] template_name
  /// @param [in] depth
  /// @param [in] body
  /// @param [out] p_matched_result_item
  /// @return 0:success or -1
  int ExtractWrapper(const ExtractItem &extract_item,
                     MatchedResultItem *p_matched_result_item);

  /// @brief clear
  void Clear(const std::string &url_template) {
    template_lock_.WrLock();
    template_map_.erase(url_template);
    template_combine_map_.erase(url_template);
    template_while_map_.erase(url_template);
    template_until_map_.erase(url_template);
    template_if_map_.erase(url_template);
    template_lock_.Unlock();
  }

 private:
  /// load config file
  /// @param [in] template_name The template name
  /// @param [in] template_file path name
  /// @param [out] p_err_info The error info when failed
  /// @return 0:success -1:not
  int LoadTemplate(const std::string &template_name,
                   const std::string &template_file,
                   DepthPlainTemplateMap *p_depth_template_map,
                   DepthTemplateCombineMap *p_depth_combine_map,
                   DepthBlockIdxMap *p_while_block_list,
                   DepthBlockIdxMap *p_until_block_list,
                   DepthBlockIdxMap *p_if_block_list,
                   std::string *p_err_info);

  /// Preprocess
  /// @param[in] match_line_list 所有配置项
  /// @param[out] p_while_map 配置项中while块的起止下标
  /// @param[out] p_if_map 配置项中if块的起止下标
  /// @param [out] p_err_info The error info when failed
  /// @return 0:success -1:not
  int Preprocess(const std::vector<PlainMatchLine> &match_line_list,
                 BlockIdxMap *p_while_map,
                 BlockIdxMap *p_until_map,
                 BlockIdxMap *p_if_map,
                 std::string *p_err_info);

  int OnUntil(const std::string &body, 
              const std::vector<PlainMatchLine> &match_line_list,
              const BlockIdxMap &if_map,
              SubResultList *p_sub_result_list);

  void OnNonLoopField(const PlainMatchLine &match_line,
                      const BlockIdxMap &if_map,
                      const std::string &body,
                      int *p_i,
                      CtrlCmd *p_cur_cmd,
                      std::stack<CtrlCmd> *p_cmd_stack,
                      size_t *p_cur_body_pos,
                      bool *p_cmd_matched,
                      SelfResult *self_result,
                      SelfResult *p_sub_result);

  /// Sep Operation, invoke in Parse
  /// @param [in] match_line The match line
  /// @param [in] body
  /// @param [in] pos The start position
  /// @return pos or string::npos
  int OnSep(const PlainMatchLine &match_line, const std::string& body, size_t pos, size_t *next_pos);

  /// Field Operation, invoke in Parse
  /// @param [in] match_line The match line
  /// @param [in] body
  /// @param [in] pos The start position
  /// @param [out] result_map
  /// @param [out] next_pos
  /// @return 0:success -1:failure
  int OnField(const PlainMatchLine &match_line,
              const std::string &body,
              size_t pos,
              SelfResult *result_map,
              size_t *next_pos);

  /// Extract from Body content, then get the result
  /// @param [in] template_name
  /// @param [in] depth
  /// @param [in] body
  /// @param [out] p_matched_result_item
  /// @return 0:success or -1
  int Extract(const ExtractItem &extract_item,
              MatchedResultItem *p_matched_result_item);

 private:
  PlainTemplateMap template_map_;
  TemplateCombineMap template_combine_map_;
  TemplateBlockIdxMap template_while_map_;
  TemplateBlockIdxMap template_until_map_;
  TemplateBlockIdxMap template_if_map_;
  /// protect the above data structure
  RWLock template_lock_;
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_PLAIN_EXTRACTOR_H_
