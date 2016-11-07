/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/base_extractor.h
 * @namespace ganji::crawler::conf_extractor::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-11
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/extractor/base_extractor.h"

#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <stack>
#include <utility>
#include <algorithm>

#include "util/text/text.h"
#include "util/file/file.h"
#include "util/system/system.h"
#include "util/time/time.h"
#include "util/log/thread_fast_log.h"

#include "conf_crawler/extractor/extractor_config.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using std::string;
using std::vector;
using std::list;
using std::map;
using std::pair;

using boost::smatch;
using boost::regex;
using boost::regex_search;
using boost::regex_match;
using boost::regex_error;

namespace GFile = ::ganji::util::file;
namespace Text = ::ganji::util::text::Text;
namespace Time = ::ganji::util::time;
namespace System = ::ganji::util::system::System;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

int BaseExtractor::Init(ExtractorConfig *p_config) {
  p_config_ = p_config;

  int socket_timeout = p_config_->GetSocketTimeout();
  int persist_count = p_config_->GetPersistCount();
  link_base_conn_.Init(p_config->GetLinkBaseHost(),
                       p_config->GetLinkBasePort(),
                       socket_timeout,
                       persist_count);

  p_timer_thread_ = thread::Thread::CreateThread(TimerThread, this);
  p_timer_thread_->ResumeThread();

  // p_get_task_thread_ = thread::Thread::CreateThread(GetTaskThread, this);
  // p_get_task_thread_->ResumeThread();

  int svr_thread_count = p_config_->GetSvrThreadCount();
  for (int i = 0; i < svr_thread_count; i++) {
    p_svr_thread_ = thread::Thread::CreateThread(SvrThread, this);
    p_svr_thread_->ResumeThread();
  }

  return 0;
}

int BaseExtractor::GetMultipleTemplates(const string &template_path,
                                        const string &template_prefix,
                                        vector<string> *p_file_list) {
  p_file_list->clear();

  DIR *p_dir = opendir(template_path.c_str());
  if (!p_dir) {
    WriteLog(kLogFatal, "opendir[%s] failed:%s", template_path.c_str(), strerror(errno));
    return -1;
  }
  
  string multiple_str = template_prefix + string(kMultipleTemplateSuffix);
  regex multiple_regex;
  try {
    multiple_regex = regex(multiple_str);
  } catch(const regex_error &e)  {
    WriteLog(kLogFatal, "construct regex[%s] failed:%s", multiple_str.c_str(), e.what());
    closedir(p_dir);
    return -1;
  }

  string single_str = template_prefix + string(kTemplateSuffix);
  regex single_regex;
  try {
    single_regex = regex(single_str);
  } catch(const regex_error &e)  {
    WriteLog(kLogFatal, "construct regex[%s] failed:%s", single_str.c_str(), e.what());
    closedir(p_dir);
    return -1;
  }

  struct dirent *dir_ent = NULL;
  while (NULL != (dir_ent = readdir(p_dir))) {
    string file_name = dir_ent->d_name;
    /// domain.category_\d+.txt
    smatch multiple_what, single_what;
    try {
      if (regex_match(file_name, multiple_what, multiple_regex) ||
          regex_match(file_name, single_what, single_regex)) {
        string template_file = file_name;
        p_file_list->push_back(template_file);
      }
    } catch(const regex_error &e) {
      WriteLog(kLogFatal, "regex_match failed:%s", e.what());
    }
  }

  closedir(p_dir);
  if (p_file_list->empty())
    return -1;
  return 0;
}

void BaseExtractor::PushIntoExtractQueue(const ExtractItem &extract_item) {
  queue_lock_.Lock();
  extract_queue_.push(extract_item);
  queue_lock_.Unlock();
  queue_cond_.Signal();
}

int BaseExtractor::ConstructMatchRegex(const string &match_pattern, BaseMatchLine *p_match_line, string *p_err_info) {
  char err_buf[kErrBufLen];
  string regex_str;
  size_t before_pos = 0;
  size_t pos = match_pattern.find(kVarPrefix);
  while (pos != string::npos) {
    string prefix = match_pattern.substr(0, pos);
    /// the nth ()
    int number = Text::GetWordCount(prefix , "(");
    /// deescape
    int remove_number = Text::GetWordCount(prefix , "(?");
    remove_number += Text::GetWordCount(prefix , "\\(");
    regex_str += match_pattern.substr(before_pos, pos - before_pos);
    size_t end_pos = match_pattern.find(kVarSuffix, pos);
    string name = match_pattern.substr(pos+2, end_pos-pos-2);
    VarCapture var_capture;
    var_capture.name = name;
    var_capture.number = number - remove_number;
    p_match_line->var_capture_list.push_back(var_capture);
    before_pos = end_pos+2;
    pos = match_pattern.find(kVarPrefix, before_pos);
  }

  regex_str += match_pattern.substr(before_pos);
  try {
    p_match_line->match_regex = regex(regex_str);
  } catch(const regex_error &e)  {
    snprintf(err_buf, kErrBufLen, "construct regex:%s failed:%s", regex_str.c_str(), e.what());
    WriteLog(kLogFatal, "%s", err_buf);
    *p_err_info = err_buf;
    return -1;
  }

  return 0;
}

int BaseExtractor::ConstructSelfDefinedVal(const string &conf_str, BaseMatchLine *p_match_line, string *p_err_info) {
  size_t pos = conf_str.find('=');
  string name = conf_str.substr(0, pos);
  string value = conf_str.substr(pos+1);

  SelfDefinedVal self_defined_val;
  self_defined_val.name = name;
  self_defined_val.value = value;
  pos = value.find("$");
  while (pos != string::npos) {
    int number = Text::FromString<int>(value.substr(pos+1, 1), 10);
    self_defined_val.number_pos_list.push_back(pair<int, size_t>(number, pos));
    pos = value.find("$", pos+1);
  }

  p_match_line->self_defined_val_list.push_back(self_defined_val);

  return 0;
}

void *BaseExtractor::TimerThread(void *arg) {
  BaseExtractor *p_extractor = reinterpret_cast<BaseExtractor *>(arg);
  int time_slice = p_extractor->p_config_->GetTimeSlice();

  while (true) {
    Sleep::DoSleep(time_slice * 1000);
    p_extractor->TimerThreadFunc();
  }

  return NULL;
}

int BaseExtractor::TimerThreadFunc() {
  queue_lock_.Lock();
  int queue_size = extract_queue_.size();
  queue_lock_.Unlock();

  int mb = 0;
  System::GetMem(&mb);
  WriteLog(kLogNotice, "#queue:%d mem:%dMB", queue_size, mb);

  return 0;
}

void *BaseExtractor::SvrThread(void *arg) {
  BaseExtractor *p_extractor = reinterpret_cast<BaseExtractor *>(arg);
  while (true) {
    p_extractor->SvrThreadFunc();
  }

  return NULL;
}

int BaseExtractor::SvrThreadFunc() {
  queue_lock_.Lock();
  while (extract_queue_.empty()) {
    queue_lock_.Unlock();
    queue_cond_.Wait();
    queue_lock_.Lock();
  }
  ExtractItem extract_item = extract_queue_.front();
  extract_queue_.pop();
  queue_lock_.Unlock();

  MatchedResultItem matched_result_item;
  int ret = ExtractWrapper(extract_item, &matched_result_item);
  if (ret < 0) {
    matched_result_item.is_ok = false;
  }
  uint32_t start_t = Time::GetCurTimeMs();
  ret = UploadExtract(extract_item, matched_result_item);
  uint32_t end_t = Time::GetCurTimeMs();
  int diff = end_t - start_t;
  if (ret < 0) {
    WriteLog(kLogNotice, "UploadExtract()[%s] failed, elapsed:%dms", extract_item.url.c_str(), diff);
  } else {
    WriteLog(kLogNotice, "UploadExtract()[%s] OK, elapsed:%dms", extract_item.url.c_str(), diff);
  }
  return ret;
}

int BaseExtractor::UploadExtract(const ExtractItem &extract_item, const MatchedResultItem &matched_result_item) {
  int ret = 0;
  link_base_conn_.Lock();
  do {
    /// reconnect if not connected or request limit attained
    if (link_base_conn_.NeedReset()) {
      bool is_ok = link_base_conn_.Reset();
      if (!is_ok) {
        ret = -1;
        break;
      }
    }

    /// retry until success
    for (int i = 0; i < p_config_->GetReqRetryTimes(); i++) {
      try {
        link_base_conn_.Client()->upload_extract_task(extract_item, matched_result_item);
        link_base_conn_.IncrTimes();
        ret = 0;
        break;
      } catch(const TTransportException &te) {
        WriteLog(kLogNotice, "upload_extract_task(), Exception:%s [%d]", te.what(), te.getType());
      } catch(const TException &e) {
        WriteLog(kLogNotice, "upload_extract_task(), Exception:%s", e.what());
      }
      ret = -1;
      bool is_ok = link_base_conn_.Reset();
      if (!is_ok) {
        WriteLog(kLogNotice, "upload_extract_task() failed, times:%d Reset failed", i);
        break;
      } else {
        WriteLog(kLogNotice, "upload_extract_task() failed, times:%d Reset OK", i);
      }
    }
  } while (0);
  link_base_conn_.Unlock();

  return ret;
}

int BaseExtractor::FillValue(const string &value,
                             const SelfResult &self_result,
                             string *p_filled_value) {
  string var_prefix = kVarPrefix;
  string var_suffix = kVarSuffix;
  string post_param = kPostParam;

  size_t prev_pos = 0;
  while (true) {
    size_t start_pos = value.find(var_prefix, prev_pos);
    if (start_pos == string::npos)
      break;
    start_pos += var_prefix.size();
    size_t end_pos = value.find(var_suffix, start_pos);
    if (end_pos == string::npos)
      break;
    string var_name = value.substr(start_pos, end_pos-start_pos);
    string var_value;
    do {
      if (var_name != post_param) {
        SelfResult::const_iterator it = self_result.find(var_name);
        if (it == self_result.end()) {
          WriteLog(kLogNotice, "no [%s] in combine field", var_name.c_str());
          break;
        }
        if (it->second.empty()) {
          WriteLog(kLogNotice, "[%s] empty in combine field", var_name.c_str());
          break;
        }
        var_value = it->second[0];
      } else {
        var_value = var_prefix + post_param + var_suffix;
      }
    } while (0);

    *p_filled_value += value.substr(prev_pos, start_pos-var_prefix.size()-prev_pos);
    *p_filled_value += var_value;
    prev_pos = end_pos + var_suffix.size();
  }

  if (prev_pos < value.size())
    *p_filled_value += value.substr(prev_pos);

  return 0;
}

int BaseExtractor::GetTemplateDomain(const string &template_name, string *p_domain) {
  size_t pos = template_name.find(kTemplateDomainDelim);
  if (pos == string::npos)
    return -1;
  *p_domain = template_name.substr(0, pos);
  return 0;
}

int BaseExtractor::ParseTemplateName(const string &template_name, string *p_domain, string *p_category) {
  size_t pos = template_name.find(kTemplateDomainDelim);
  if (pos == string::npos)
    return -1;
  *p_domain = template_name.substr(0, pos);
  *p_category = template_name.substr(pos+1);
  return 0;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor
