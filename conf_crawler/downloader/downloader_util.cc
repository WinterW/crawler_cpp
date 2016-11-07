/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/downloader_config.h
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-01
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "downloader_util.h"

#include <string.h>

using std::string;

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader { namespace DownloaderUtil {
int GetUrlDomain(const string &url, string *domain) {
  // format http://xxx.xxx.xx
  const char *http_prefix = "http://";
  if (strncasecmp(url.c_str(), http_prefix, strlen(http_prefix)) != 0) {
    return -1;
  }
  *domain = url.substr(strlen(http_prefix));

  for (int i = 0; i < static_cast<int>(domain->size()); i++) {
    if ((*domain)[i] == '/' || (*domain)[i] == ':') {
      *domain = (*domain).substr(0, i);
      break;
    }
  }
  return 0;
}

int GetMainDomain(const string &url, string *main_domain) {
  string domain;
  if (GetUrlDomain(url, &domain) < 0)
    return -1;

  /// 如果domain是IP,则直接返回
  /// 判断方法: 字符串中有一个字符是字母，说明不是IP
  bool is_ip = true;
  for (int i = 0; i < static_cast<int>(domain.size()); i++) {
    char ch = domain[i];
    if (isalpha(ch)) {
      is_ip = false;
      break;
    }
  }
  if (is_ip)
    return -1;

  /// http://ganji.com
  /// http://ganji.com.cn
  /// http://www.ganji.com
  /// http://www.fang.ganji.com
  /// http://www.ganji.com.cn"
  /// .com .cn .net .org .com.cn
  /// 获得一级域名
  size_t tpos1 = string::npos;
  for (int i = 0; i < static_cast<int>(sizeof(kTopDomain)/sizeof(kTopDomain[0])); i++) {
    tpos1 = domain.find(kTopDomain[i]);
    if (tpos1 != string::npos) {
      break;
    }
  }
  /// 如果是 www.ganji.com  www.ganji.com.cn 这样 就从.com 往前找"."
  if (tpos1 != string::npos) {
    tpos1 -= 1;
    size_t tpos2 = string::npos;
    tpos2 = domain.rfind(".", tpos1);
    if (tpos2 != string::npos) {
      domain = domain.substr(tpos2+1, domain.length());
    }
  } else {
    size_t tpos2 = string::npos;
    tpos2 = domain.rfind(".");
    tpos2 -= 1;
    /// 如果不是www.ganji.uk 这样的域名 就找到 倒数 第二个"."
    tpos2 = domain.rfind("." , tpos2);
    /// 如果得到第二个"."就截取 主域名 ， 否则 本身就是主域名
    if (tpos2 != string::npos) {
      domain = domain.substr(tpos2+1, domain.length());
    }
  }

  if (domain.empty()) {
    return -1;
  } else {
    *main_domain = domain;
    return 0;
  }
}
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader::DownloaderUtil

