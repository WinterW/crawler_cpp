/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_util.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-25
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_UTIL_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_LINK_UTIL_H_

#include <string>

#include "conf_crawler/link_base/struct_def.h"
#include "conf_crawler/link_base/conf_crawler_types.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base { namespace LinkUtil {
/// Whether url is absolute url
bool IsAbsUrl(const std::string &url);

/// Get abs url from relative form
int Relative2AbsUrl(const std::string &referer, const std::string &rel_url, std::string *abs_url);

int ParseDownloaderTypeList(const std::string &downloader_type, BaseSeedItem *p_seed); 

int ParseDownloaderType(const std::string &downloader_type, DownloaderType::type *p_type);

int ParseHeaderFieldsTypeList(const std::string &header_fields_type, BaseSeedItem *p_seed);

///add by wangsj,Parse Download Interval 
int ParseDownIntervalTypeList(const std::string &down_interval_type, BaseSeedItem *p_seed);

/// Parse header fields type
int ParseHeaderFieldsType(const std::string &header_fields_type, HeaderFieldsType::type *p_type);

/// Parse extract type into list
int ParseTemplateTypeList(const std::string &template_type, BaseSeedItem *p_seed);

int ParseTemplateType(const std::string &template_type, TemplateType::type *p_ex_type);
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base::LinkUtil

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_BASE_LINK_H_
