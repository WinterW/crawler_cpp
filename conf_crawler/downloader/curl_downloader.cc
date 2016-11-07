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

#include "conf_crawler/downloader/curl_downloader.h"

#include <netdb.h>

#include <vector>

#include "util/text/text.h"
#include "util/time/time.h"
#include "util/log/thread_fast_log.h"
#include "util/net/http_opt.h"
#include "util/compress/gzip.h"

#include "conf_crawler/downloader/downloader_config.h"

using std::string;
using std::vector;

namespace Http = ::ganji::util::net::Http;
namespace Text = ::ganji::util::text::Text;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
namespace Time = ganji::util::time;
namespace Gzip = ganji::util::compress::Gzip;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
int CurlDownloader::Init(DownloaderConfig *p_config) {
  p_config_ = p_config;
  if (!p_config_) {
    WriteLog(kLogFatal, "p_config NULL");
    return -1;
  }

  p_curl_ = curl_easy_init();
  if (!p_curl_) {
    WriteLog(kLogFatal, "curl_easy_init failed:%s", strerror(errno));
    return -1;
  }

  SetDebug(p_config->IsDebugMode());
  /// XXX completely disable dns caching
  // curl_easy_setopt(p_curl_, CURLOPT_DNS_CACHE_TIMEOUT, 0);
  /// XXX cache dns forever
  curl_easy_setopt(p_curl_, CURLOPT_DNS_CACHE_TIMEOUT, -1);
  curl_easy_setopt(p_curl_, CURLOPT_ERRORBUFFER, err_buf_);
  curl_easy_setopt(p_curl_, CURLOPT_WRITEDATA, &body_);
  curl_easy_setopt(p_curl_, CURLOPT_WRITEFUNCTION, &process_data);
  curl_easy_setopt(p_curl_, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(p_curl_, CURLOPT_FOLLOWLOCATION, 1);
  int curl_opt_max_dirs = p_config->GetCurlOptMaxRedirs();
  curl_easy_setopt(p_curl_, CURLOPT_MAXREDIRS, curl_opt_max_dirs);

  if(p_config->IsProxyMode()) {
    curl_easy_setopt(p_curl_, CURLOPT_PROXY, p_config_->GetProxyIp().c_str());
    //curl_easy_setopt(p_curl_, CURLOPT_PROXY, "192.168.22.200:3300");
  }

  const string &local_interface = p_config->GetLocalInterface();
  if (!local_interface.empty()) {
    curl_easy_setopt(p_curl_, CURLOPT_INTERFACE, local_interface.c_str());
  }

  /// init conn with header fields svr
  int socket_timeout = p_config_->GetSocketTimeout();             
  int persist_count = p_config_->GetPersistCount();               
  header_fields_conn_.Init(p_config_->HeaderFieldsSvrHost(), p_config_->HeaderFieldsSvrPort(), socket_timeout, persist_count);

  return 0;
}

void CurlDownloader::SetDebug(bool debug) {
  if (debug)
    curl_easy_setopt(p_curl_, CURLOPT_VERBOSE, 1);
  else
    curl_easy_setopt(p_curl_, CURLOPT_VERBOSE, 0);
}

void CurlDownloader::SetNobody(bool is_nobody) {
  if (is_nobody)
    curl_easy_setopt(p_curl_, CURLOPT_NOBODY, 1);
  else
    curl_easy_setopt(p_curl_, CURLOPT_NOBODY, 0);
}

void CurlDownloader::SetTimeoutMs(size_t time_out) {
  curl_easy_setopt(p_curl_, CURLOPT_TIMEOUT_MS, time_out);
}

int CurlDownloader::Perform(const HttpReqItem &req_item, string *p_body) {
  body_.clear();
  p_body->clear();
  /// XXX default to non-post request
  curl_easy_setopt(p_curl_, CURLOPT_POST, 0);

  string url = req_item.url_;
  const string &referer = req_item.referer_;
  const string &ua = req_item.ua_;
  HeaderFieldsType::type header_fields_type = req_item.header_fields_type_;
  int depth = req_item.depth_;

  /// process POST request
  string post_fields;
  size_t pos = url.find(kPostParam);
  if (pos != string::npos) {
    post_fields = url.substr(pos+strlen(kPostParam));
    post_fields = Http::EscapeURL(post_fields);
    url = url.substr(0, pos);
  }

  size_t time_out = req_item.time_out_;
  const string &ip = req_item.ip_;

  if (ip.empty()) {
    WriteLog(kLogFatal, "Perform[%s] ip empty", url.c_str());
    return -1;
  }

  /// parse url to get protol/domain/port
  string protocol, domain;
  int port;
  int ret = Http::ParseUrl(url, &protocol, &domain, &port);
  if (ret < 0) {
    WriteLog(kLogFatal, "ParseUrl[%s] failed", url.c_str());
    return -1;
  }
  if (port == 0) {
    struct servent *s_ptr = getservbyname(protocol.c_str(), NULL);
    if (!s_ptr) {
      WriteLog(kLogFatal, "getservbyname[%s] failed:%s", protocol.c_str(), strerror(errno));
      return -1;
    }
    int s_port = s_ptr->s_port;
    port = ntohs(s_port);
  }

  /// use dns cache
 
  /// HOST:PORT:ADDRESS
  string host_ip = domain + ":" + Text::IntToStr(port) + ":" + ip;
  struct curl_slist *p_list = NULL;
  p_list = curl_slist_append(p_list, host_ip.c_str()); 
  curl_easy_setopt(p_curl_, CURLOPT_RESOLVE, p_list);

  curl_easy_setopt(p_curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(p_curl_, CURLOPT_TIMEOUT, time_out);

  /// request header_fields_svr
  vector<string> header_fields_list;
  if (header_fields_type == HeaderFieldsType::UPDATE_ALL_TYPE ||
      header_fields_type == HeaderFieldsType::UPDATE_PART_TYPE) {
    int is_ok = GetHeaderFields(url, depth, &header_fields_list);
    if (is_ok < 0) {
      curl_slist_free_all(p_list);
      WriteLog(kLogWarning, "GetHeaderFields[%s][%d][%d] failed", url.c_str(), depth, header_fields_type);
      return -1;
    }
  }

  struct curl_slist *headers = NULL;
  if (header_fields_type == HeaderFieldsType::UPDATE_ALL_TYPE) {
    /// TODO remove field `Accept'???
    headers = curl_slist_append(headers, "Accept:");
    for (vector<string>::iterator it = header_fields_list.begin();
        it != header_fields_list.end(); ++it)
      headers = curl_slist_append(headers, it->c_str());
    curl_easy_setopt(p_curl_, CURLOPT_HTTPHEADER, headers); 
  } else {
    curl_easy_setopt(p_curl_, CURLOPT_REFERER, referer.c_str());
    curl_easy_setopt(p_curl_, CURLOPT_USERAGENT, ua.c_str());
    /// XXX CURLOPT_ACCEPT_ENCODING was called CURLOPT_ENCODING before 7.21.6
    curl_easy_setopt(p_curl_, CURLOPT_ENCODING, kGzip);

    for (vector<string>::iterator it = header_fields_list.begin();
        it != header_fields_list.end(); ++it)
      headers = curl_slist_append(headers, it->c_str());
    curl_easy_setopt(p_curl_, CURLOPT_HTTPHEADER, headers); 
  }

  if (!post_fields.empty()) {
    curl_easy_setopt(p_curl_, CURLOPT_POST, 1);
    curl_easy_setopt(p_curl_, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(p_curl_, CURLOPT_POSTFIELDSIZE, post_fields.size());
  }

  CURLcode curl_code = curl_easy_perform(p_curl_);
  curl_slist_free_all(p_list);
  if (headers)
    curl_slist_free_all(headers);

  /// XXX automatic decompression is explicitly enabled in zlib-enabled builds of libcurl, so no need to inflate explicitly
  /// inflate body
  // string inflate_body;
  // int inf_ret = Gzip::Inflate(body_.c_str(), body_.size(), &inflate_body);

  double dns_time = 0, connect_time = 0, total_time = 0;
  curl_easy_getinfo(p_curl_, CURLINFO_NAMELOOKUP_TIME, &dns_time);
  curl_easy_getinfo(p_curl_, CURLINFO_CONNECT_TIME, &connect_time);
  curl_easy_getinfo(p_curl_, CURLINFO_TOTAL_TIME, &total_time);
  WriteLog(kLogDebug, "[%s] time dns[%f] conn[%f] total[%f]", url.c_str(), dns_time, connect_time, total_time);

  if (curl_code == 0) {
    long http_code = 0;
    curl_easy_getinfo(p_curl_, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code >= 200 && http_code < 300 && http_code != 202 && curl_code != CURLE_ABORTED_BY_CALLBACK) {
      WriteLog(kLogDebug, "Perform[%s] header[%d] depth[%d] OK, code:%lu len(body):%lu", url.c_str(), header_fields_type, depth, http_code, body_.size());
      *p_body = body_;
      // if (inf_ret == 0)
      //   *p_body = inflate_body;
      // else
      //   *p_body = body_;
      return 0;
    } else {
      WriteLog(kLogWarning, "Perform[%s] failed, code:%lu", url.c_str(), http_code);
      return -1;
    }
  }
  WriteLog(kLogWarning, "Perform[%s] failed:%s", url.c_str(), err_buf_);
  return -1;
}

size_t CurlDownloader::process_data(void *buffer, size_t size, size_t nmemb, void *user_p) {
  string *body = reinterpret_cast<string *>(user_p);
  *body += string(reinterpret_cast<char *>(buffer), size * nmemb);
  return size * nmemb;
}

int CurlDownloader::GetHeaderFields(const string &url, int depth, vector<string> *p_header_list) {
  int ret = 0;
  header_fields_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (header_fields_conn_.NeedReset()) {
      bool is_ok = header_fields_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetRetryTimes(); i++) {
      try {
        header_fields_conn_.Client()->get_header_fields(*p_header_list, url, depth);
        header_fields_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(...) {
        ret = -1;
        WriteLog(kLogFatal, "get_header_fields() failed");
        bool is_ok = header_fields_conn_.Reset();
        if (!is_ok) {
          break;
        }
      }
    }
  } while (0);
  header_fields_conn_.Unlock();

  return ret;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader
