/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/curl_downloader.h
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-27
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_CURL_DOWNLOADER_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_CURL_DOWNLOADER_H_

#include <curl/curl.h>

#include <string>
#include <vector>

#include "global.h"
#include "conf_crawler/common/long_short_conn.h"
#include "conf_crawler/downloader/struct_def.h"
#include "conf_crawler/downloader/HeaderFieldsService.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
class DownloaderConfig;

using ganji::crawler::conf_crawler::LongShortConn;

/**
 * @class CurlGlobal
 * @brief 维护curl全局变量
 */
class CurlGlobal {
 public:
  CurlGlobal() {
    curl_global_init(CURL_GLOBAL_ALL);
  }

  ~CurlGlobal() {
    curl_global_cleanup();
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(CurlGlobal);
};

/**
 * @class CurlDownloader
 * @brief 使用curl进行下载
 */
class CurlDownloader {
 public:
  CurlDownloader()
    : p_curl_(NULL) {
  }

  ~CurlDownloader() {
    if (p_curl_) {
      curl_easy_cleanup(p_curl_);
    }
  }

  /// @brief init CURL
  int Init(DownloaderConfig *p_config);

  /// @brief set/reset debug mode
  /// @param[in] debug true:debug mode
  void SetDebug(bool debug);

  /// @brief set/reset nobody mode
  /// @param[in] is_nobody true: HEAD request
  void SetNobody(bool is_nobody);

  /// @brief set timeout in ms
  /// @param[in] time_out milliseconds
  void SetTimeoutMs(size_t time_out);

  /// @brief perform a http request
  /// @param[in] req_item request parameters
  /// @param[out] p_body requested body
  /// @ret 0:success -1:failure
  int Perform(const HttpReqItem &req_item, std::string *p_body);

 private:
  /// @brief parameter for `CURLOPT_WRITEFUNCTION'
  static size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p);

  /// Get header fields
  int GetHeaderFields(const std::string &url, int depth, std::vector<std::string> *p_header_list);

 private:
  DownloaderConfig *p_config_;

  CURL *p_curl_;

  LongShortConn<HeaderFieldsServiceClient> header_fields_conn_;      ///< connection with header fields svr

  char err_buf_[CURL_ERROR_SIZE+1];
  std::string body_;                ///< parameter for `CURLOPT_WRITEDATA'

 private:
  DISALLOW_COPY_AND_ASSIGN(CurlDownloader);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_CURL_DOWNLOADER_H_
