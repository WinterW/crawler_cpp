/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/relay_extractor.cc
 * @namespace ganji::crawler::conf_extractor::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-24
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/extractor/relay_extractor.h"

#include <assert.h>
#include <stack>
#include <utility>
#include <algorithm>

#include "util/text/text.h"
#include "util/file/file.h"
#include "util/time/time.h"
#include "util/log/thread_fast_log.h"
#include "conf_crawler/extractor/extractor_config.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using std::string;
using std::vector;
using std::list;
using std::map;
using std::pair;

namespace GFile = ::ganji::util::file;
namespace Text = ::ganji::util::text::Text;
namespace Time = ::ganji::util::time;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

int RelayExtractor::Init(ExtractorConfig *p_config) {
  int ret = BaseExtractor::Init(p_config);
  if (ret < 0)
    return -1;

  int socket_timeout = p_config_->GetSocketTimeout();             
  int persist_count = p_config_->GetPersistCount();               
  const string &relay_svr_host = p_config_->RelaySvrHost();
  int relay_svr_port = p_config_->RelaySvrPort();
  relay_svr_conn_.Init(relay_svr_host, relay_svr_port, socket_timeout, persist_count);

  return 0;
}

int RelayExtractor::ExtractWrapper(const ExtractItem &extract_item,
                                   MatchedResultItem *p_matched_result_item) {
  int ret = 0;
  relay_svr_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (relay_svr_conn_.NeedReset()) {
      bool is_ok = relay_svr_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetReqRetryTimes(); i++) {
      try {
        relay_svr_conn_.Client()->extract_sync(*p_matched_result_item, extract_item);
        relay_svr_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(const TTransportException &te) {
        ret = -1;
        WriteLog(kLogFatal, "extract_sync() failed, Exception:%s [%d]", te.what(), te.getType());
      } catch(const TException &e) {
        ret = -1;
        WriteLog(kLogFatal, "extract_sync() failed, Exception:%s", e.what());
      }
      bool is_ok = relay_svr_conn_.Reset();
      if (!is_ok) {
        break;
      }
    }
  } while (0);
  relay_svr_conn_.Unlock();

  return ret;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor
