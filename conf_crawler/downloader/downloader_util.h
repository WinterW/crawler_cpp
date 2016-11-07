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

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_UTIL_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_UTIL_H_

#include <string>

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader { namespace DownloaderUtil {
const char kTopDomain[][10] = {
  ".com",  ///< 商业机构，任何人都可以注册；
  ".edu",  ///< 教育机构；
  ".gov",  ///< 政府部门；
  ".int",  ///< 国际组织；
  ".mil",  ///< 美国军事部门；
  ".net",  ///< 网络组织，例如因特网服务商和维修商，现在任何人都可以注册；
  ".org",  ///< 非盈利组织，任何人都可以注册；
  ".biz",  ///< 商业；
  ".info",  ///< 网络信息服务组织；
  ".pro",  ///< 用于会计、律师和医生。；
  ".name",  ///< 用于个人；
  ".museum",  ///< 用于博物馆；
  ".coop",  ///< 用于商业合作团体；
  ".aero",  ///< 用于航空工业；
  ".xxx",  ///< 用于成人、色情网站；
  ".idv"
};


int GetUrlDomain(const std::string &url, std::string *domain);

int GetMainDomain(const std::string &url, std::string *main_domain);
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader::DownloaderUtil

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_UTIL_H_
