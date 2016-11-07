/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/dev_link_base.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/link_base/dev_link_base.h"

#include <assert.h>

#include "ganji/crawler/conf_crawler/link_base/link_config.h"

#include "ganji/util/db/db_pool.h"
#include "ganji/util/thread/sleep.h"
#include "ganji/util/text/text.h"
#include "ganji/util/file/file.h"
#include "ganji/util/time/time.h"
#include "ganji/util/log/thread_fast_log.h"

namespace Time = ::ganji::util::time;
namespace Text = ::ganji::util::text::Text;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;
using ::ganji::util::db::DBPool;
using ::ganji::util::db::SqlResult;

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
int DevLinkBase::Init(LinkConfig *p_config) {
  if (!p_config) {
    WriteLog(kLogFatal, "link config NULL");
    return -1;
  }

  p_config_ = p_config;

  mongo_.Init(p_config_);

  dc_conn_.Init(p_config_->GetDcHost(), p_config_->GetDcPort(),
                p_config_->GetLongConnSockTimeout(), p_config_->GetLongConnCheckInterval());
  extractor_conn_.Init(p_config_->GetExtractorHost(), p_config_->GetExtractorPort(),
                       p_config_->GetLongConnSockTimeout(), p_config_->GetLongConnCheckInterval());

  return 0;
}

int DevLinkBase::DevUrl(const DevUrlItem &dev_url_item, MatchedResultItem *matched_result_item) {
  const string &url = dev_url_item.url;
  const string &url_template = dev_url_item.url_template;
  DownloadedBodyItem downloaded_body_item;
  int to_download = dev_url_item.to_download;

  int ret = 0;
  string body;
  /// 不下载，从mongodb中获取
  if (to_download == 0) {
    ret = mongo_.QueryDevBody(dev_url_item.url,
                              dev_url_item.url_template,
                              dev_url_item.depth,
                              &body);
    if (ret < 0) {
      snprintf(err_buf_, kErrBufLen, "query dev body:%s template:%s failed", url.c_str(), url_template.c_str());
      WriteLog(kLogFatal, "%s", err_buf_);
      matched_result_item->is_ok = false;
      matched_result_item->err_info = err_buf_;
      return -1;
    }
  } else {
    /// 同步下载
    ret = DownloadSync(dev_url_item, &downloaded_body_item);
    if (ret < 0 || !downloaded_body_item.is_ok) {
      snprintf(err_buf_, kErrBufLen, "download sync:%s template:%s failed", url.c_str(), url_template.c_str());
      WriteLog(kLogFatal, "%s", err_buf_);
      matched_result_item->is_ok = false;
      matched_result_item->err_info = err_buf_;
      return -1;
    }
    body = downloaded_body_item.body;
    
    /// 存储到mongodb中
    ret = mongo_.StoreDevBody(dev_url_item.url,
                              dev_url_item.url_template,
                              dev_url_item.depth,
                              downloaded_body_item.body);
    if (ret < 0) {
      snprintf(err_buf_, kErrBufLen, "store dev body:%s template:%s failed", url.c_str(), url_template.c_str());
      WriteLog(kLogNotice, "%s", err_buf_);
    }
  }

  /// 同步抽取
  ret = ExtractSync(dev_url_item, body, matched_result_item);
  if (ret < 0 || !matched_result_item->is_ok) {
    snprintf(err_buf_, kErrBufLen, "extract sync:%s template:%s failed", url.c_str(), url_template.c_str());
    WriteLog(kLogFatal, "%s", err_buf_);
    matched_result_item->is_ok = false;
    matched_result_item->err_info = string(err_buf_) + "<br>" + matched_result_item->err_info;
    return -1;
  }

  matched_result_item->is_ok = true;

  return 0;
}

int DevLinkBase::DownloadSync(const DevUrlItem &dev_url_item, DownloadedBodyItem *downloaded_body_item) {
  UrlItem url_item;
  url_item.url = dev_url_item.url;
  url_item.referer = url_item.url;
  url_item.down_friendly = false;

  /// 长连接异常
  dc_conn_.sock_lock_.Lock();
  if (!dc_conn_.p_client_) {
    dc_conn_.sock_lock_.Unlock();
    return -1;
  }

  /// 向dc转发
  try {
    dc_conn_.p_client_->download_sync(*downloaded_body_item, url_item);
  } catch(...) {
    dc_conn_.Clear();
    dc_conn_.sock_lock_.Unlock();
    WriteLog(kLogFatal, "download() failed");
    return -1;
  }
  dc_conn_.sock_lock_.Unlock();

  return 0;
}

int DevLinkBase::ExtractSync(const DevUrlItem &dev_url_item,
                             const string &body,
                             MatchedResultItem *matched_result_item) {
  int ret = 0;
  do {
    extractor_conn_.sock_lock_.Lock();
    /// 长连接异常
    if (!extractor_conn_.p_client_) {
      extractor_conn_.sock_lock_.Unlock();
      ret = -1;
      break;
    }

    ExtractItem extract_item;
    extract_item.url = dev_url_item.url;
    extract_item.url_template = dev_url_item.url_template;
    extract_item.depth = dev_url_item.depth;
    extract_item.body = body;
    /// 向extractor转发
    try {
      extractor_conn_.p_client_->extract_sync(*matched_result_item, extract_item);
    } catch(...) {
      extractor_conn_.Clear();
      extractor_conn_.sock_lock_.Unlock();
      WriteLog(kLogFatal, "extract() failed");
      ret = -1;
      break;
    }
  } while (0);
  extractor_conn_.sock_lock_.Unlock();

  return ret;
}

int DevLinkBase::ExtractUpdateConf() {
  int ret = 0;
  do {
    extractor_conn_.sock_lock_.Lock();
    /// 长连接异常
    if (!extractor_conn_.p_client_) {
      extractor_conn_.sock_lock_.Unlock();
      ret = -1;
      break;
    }

    /// 向extractor转发
    try {
      extractor_conn_.p_client_->update_conf();
    } catch(...) {
      extractor_conn_.Clear();
      extractor_conn_.sock_lock_.Unlock();
      WriteLog(kLogFatal, "ExtractLoadConf() failed");
      ret = -1;
      break;
    }
  } while (0);
  extractor_conn_.sock_lock_.Unlock();

  return ret;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base

