/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/relay_extractor.h
 * @namespace ganji::crawler::conf_crawler::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-24
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_RELAY_EXTRACTOR_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_RELAY_EXTRACTOR_H_

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
#include "conf_crawler/extractor/base_extractor.h"

#include "conf_crawler/extractor/conf_crawler_types.h"
#include "conf_crawler/extractor/ExtractorService.h"
#include "conf_crawler/common/long_short_conn.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using ganji::util::thread::Thread;
using ganji::util::thread::Mutex;
using ganji::util::thread::RWLock;
using ganji::util::thread::Condition;

using ganji::crawler::conf_crawler::LongShortConn;

class ExtractorConfig;

/**
 * @class RelayExtractor
 * @brief extract by relay to next extractor svr
 */
class RelayExtractor: public BaseExtractor {
 public:
  RelayExtractor() {
  }

  ~RelayExtractor() {
  }

  /// Init
  /// @param[in] p_config ExtractorConfig
  /// @return 0:success -1:failure
  int Init(ExtractorConfig *p_config);

  /// @brief load template file
  int LoadTemplate(const std::string &template_name, std::string *p_err_info) {
    return 0;
  }

  int LoadTemplate(const std::string &template_name, const std::string &template_file, std::string *p_err_info) {
    return 0;
  }

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
  }

 private:
  LongShortConn<ExtractorServiceClient> relay_svr_conn_;      ///< connection with relay extractor svr

  char err_buf_[kErrBufLen];
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_RELAY_EXTRACTOR_H_
