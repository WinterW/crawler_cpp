/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/link_util.cc
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-25
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "ganji/crawler/conf_crawler/link_base/link_util.h"

#include <vector>

#include "ganji/util/text/text.h"
#include "ganji/util/net/http_opt.h"                              
#include "ganji/util/log/thread_fast_log.h"

using std::string; 
using std::vector;

namespace Http = ::ganji::util::net::Http;                        
namespace Text = ::ganji::util::text::Text;                       
namespace FastLog = ::ganji::util::log::ThreadFastLog;            
using FastLog::WriteLog;
using FastLog::kLogFatal;                                         
using FastLog::kLogDebug;                                         
using FastLog::kLogNotice;                                        
using FastLog::kLogWarning;  

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base { namespace LinkUtil {
bool IsAbsUrl(const string &url) {
  // format http://xxx.xxx.xx
  const char *http_prefix = "http://";
  if (strncasecmp(url.c_str(), http_prefix, strlen(http_prefix)) != 0) {
    return false;
  }
  return true;
}

/// TODO `.', `..' not implemented
int Relative2AbsUrl(const string &referer, const string &rel_url, string *abs_url) {
  if (referer.empty() || rel_url.empty())
    return -1;

  string new_rel_url = rel_url;
  Text::ReplaceStrStr("&amp;", "&", &new_rel_url);
  Text::ReplaceStrStr("&gt;", ">", &new_rel_url);
  Text::ReplaceStrStr("&lt;", "<", &new_rel_url);
  if (IsAbsUrl(new_rel_url)) {
    *abs_url = new_rel_url;
    return 0;
  }

  if (rel_url[0] != '/') {
    /// get dir name of referer
    size_t last_slash_pos = referer.rfind('/');
    string dir_name;
    if (last_slash_pos == string::npos) {
      WriteLog(kLogFatal, "Relative2AbsUrl[%s] invalid referer", referer.c_str());
      return -1;
    }
    if (last_slash_pos <= strlen("http://"))
      dir_name = referer + '/';
    else
      dir_name = referer.substr(0, last_slash_pos+1);

    /// remove prefix `/'
    if (!new_rel_url.empty() && new_rel_url[0] == '/')
      new_rel_url = new_rel_url.substr(1);

    *abs_url = dir_name + new_rel_url;

    return 0;
  }

  abs_url->clear();

  /// url domain
  string url_domain;
  int ret = Http::GetUrlDomain(referer, &url_domain);
  if (ret < 0)
    return -1;

  if (!new_rel_url.empty() && new_rel_url[0] != '/') {
    char last_ch = referer[referer.size()-1];
    if (last_ch != '/') {
      size_t next_slash_pos = referer.rfind('/');
      if (next_slash_pos == string::npos)
        return -1;
      if (next_slash_pos <= strlen("http://")) {
        *abs_url = "http://" + url_domain + "/" + new_rel_url;
      } else {
        *abs_url = referer.substr(0, next_slash_pos+1) + new_rel_url;
      }
    } else {
      *abs_url = referer + new_rel_url;
    }
    return 0;
  }

  *abs_url = "http://" + url_domain + new_rel_url;
  return 0;
}

int ParseDownloaderTypeList(const string &downloader_type, BaseSeedItem *p_seed) {
  int max_depth = p_seed->max_depth_;
  vector<string> seg_list;
  Text::Segment(downloader_type, kTypeDelim, &seg_list);

  /// Each depth is with downloader_type for downloader_type without delimeter
  if (seg_list.size() == 1) {
    DownloaderType::type ex_type;
    if (ParseDownloaderType(downloader_type, &ex_type) < 0)
      return -1;
    for (int i = 0; i <= max_depth; i++)
      p_seed->downloader_type_list_.push_back(ex_type);
    return 0;
  }

  if (static_cast<int>(seg_list.size()) != max_depth+1)
    return -1;

  for (vector<string>::iterator it = seg_list.begin();
      it != seg_list.end(); ++it) {
    const string &each_type = *it;
    DownloaderType::type ex_type = DownloaderType::type::NONE_TYPE;
    if (ParseDownloaderType(each_type, &ex_type) < 0) {
      return -1;
    }
    p_seed->downloader_type_list_.push_back(ex_type);
  }

  return 0;
}

int ParseDownloaderType(const string &downloader_type, DownloaderType::type *p_type) {
  int downloader_type_num = Text::StrToInt(downloader_type);
  int max_num = DownloaderType::type::DOWNLOADER_TYPE_MAX;
  if (downloader_type_num >= max_num) {
    return -1;
  }
  *p_type = static_cast<DownloaderType::type>(downloader_type_num);

  return 0;
}

///add by wangsj
int ParseDownIntervalTypeList(const string &down_interval_type, BaseSeedItem *p_seed) {
  int max_depth = p_seed->max_depth_;
  vector<string> seg_list;
  Text::Segment(down_interval_type, kTypeDelim, &seg_list);
  int last_interval = 0;

  /// Each interval is same for downloader_interval_type without delimeter
  if (seg_list.size() == 1) {
    int interval = Text::StrToInt(down_interval_type);
    if(interval < 0){
      return -1;
    }
    for (int i = 0; i <= max_depth+1; i++)
      p_seed->down_interval_.push_back(interval);
    return 0;
  }

  for (vector<string>::iterator it = seg_list.begin();
      it != seg_list.end(); ++it) {
    int interval = Text::StrToInt(*it);
    if(interval < 0){
      return -1;
    }
    p_seed->down_interval_.push_back(interval);
    last_interval = interval;
  }

  for (int i = seg_list.size(); i <= max_depth+1; i ++ )
  {
    p_seed->down_interval_.push_back(last_interval);    
  }

  return 0;
}

int ParseHeaderFieldsTypeList(const string &header_fields_type, BaseSeedItem *p_seed) {
  int max_depth = p_seed->max_depth_;
  vector<string> seg_list;
  Text::Segment(header_fields_type, kTypeDelim, &seg_list);

  /// Each depth is with header_fields_type for header_fields_type without delimeter
  if (seg_list.size() == 1) {
    HeaderFieldsType::type ex_type;
    if (ParseHeaderFieldsType(header_fields_type, &ex_type) < 0)
      return -1;
    for (int i = 0; i <= max_depth; i++)
      p_seed->header_fields_type_list_.push_back(ex_type);
    return 0;
  }

  if (static_cast<int>(seg_list.size()) != max_depth+1)
    return -1;

  for (vector<string>::iterator it = seg_list.begin();
      it != seg_list.end(); ++it) {
    const string &each_type = *it;
    HeaderFieldsType::type ex_type = HeaderFieldsType::type::NONE_TYPE;
    if (ParseHeaderFieldsType(each_type, &ex_type) < 0) {
      return -1;
    }
    p_seed->header_fields_type_list_.push_back(ex_type);
  }

  return 0;
}

int ParseHeaderFieldsType(const string &header_fields_type, HeaderFieldsType::type *p_type) {
  int header_fields_type_num = Text::StrToInt(header_fields_type);
  int max_num = HeaderFieldsType::type::HEADER_FIELDS_TYPE_MAX;
  if (header_fields_type_num >= max_num) {
    return -1;
  }
  *p_type = static_cast<HeaderFieldsType::type>(header_fields_type_num);

  return 0;
}

int ParseTemplateTypeList(const string &template_type, BaseSeedItem *p_seed) {
  int max_depth = p_seed->max_depth_;
  vector<string> seg_list;
  Text::Segment(template_type, kTypeDelim, &seg_list);

  /// Each depth is with template_type for template_type without delimeter
  if (seg_list.size() == 1) {
    TemplateType::type ex_type;
    if (ParseTemplateType(template_type, &ex_type) < 0)
      return -1;
    for (int i = 0; i <= max_depth; i++)
      p_seed->template_type_list_.push_back(ex_type);
    return 0;
  }

  if (static_cast<int>(seg_list.size()) != max_depth+1)
    return -1;

  for (vector<string>::iterator it = seg_list.begin();
      it != seg_list.end(); ++it) {
    const string &each_type = *it;
    TemplateType::type ex_type = TemplateType::type::NONE_TYPE;
    if (ParseTemplateType(each_type, &ex_type) < 0) {
      return -1;
    }
    p_seed->template_type_list_.push_back(ex_type);
  }

  return 0;
}

int ParseTemplateType(const string &template_type, TemplateType::type *p_ex_type) {
  int template_type_num = Text::StrToInt(template_type);
  int max_num = TemplateType::type::TEMPLATE_TYPE_MAX;
  if (template_type_num >= max_num) {
    return -1;
  }
  *p_ex_type = static_cast<TemplateType::type>(template_type_num);

  return 0;
}
}}}}};  ///< end of namespace ganji::crawler::conf_crawler::link_base::LinkUtil

