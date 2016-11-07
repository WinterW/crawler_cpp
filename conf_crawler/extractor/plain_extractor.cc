/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_extractor/plain_extractor.cc
 * @namespace ganji::crawler::conf_extractor::extractor
 * @version 1.0
 * @author  lvwei(original author)
 * @author  lisizhong(transformed by)
 * @date    2011-07-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/extractor/plain_extractor.h"

#include <assert.h>
#include <stack>
#include <utility>
#include <algorithm>

#include "util/text/text.h"
#include "util/file/file.h"
#include "util/time/time.h"
#include "util/log/thread_fast_log.h"

#include "conf_crawler/extractor/extractor_config.h"
#include "conf_crawler/extractor/extractor_util.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using std::string;
using std::vector;
using std::map;
using std::stack;
using std::pair;

using boost::smatch;
using boost::regex;
using boost::regex_search;
using boost::regex_error;

namespace GFile = ::ganji::util::file;
namespace Text = ::ganji::util::text::Text;
namespace Time = ::ganji::util::time;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

int PlainExtractor::LoadTemplate(const string &template_name, string *p_err_info) {
  string domain;
  if (GetTemplateDomain(template_name, &domain) < 0) {
    WriteLog(kLogFatal, "invalid template[%s], no domain", template_name.c_str());
    return -1;
  }
  /// TODO 修改为multiple template
  string template_path = p_config_->TemplatePath() + "/" +
                         string(kPlainPath) + "/" +
                         domain + "/" +
                         template_name +
                         ".txt";
  DepthPlainTemplateMap depth_template_map;
  DepthTemplateCombineMap depth_combine_map;
  DepthBlockIdxMap depth_while_map, depth_until_map, depth_if_map;
  int ret = LoadTemplate(template_name,
                         template_path,
                         &depth_template_map,
                         &depth_combine_map,
                         &depth_while_map,
                         &depth_until_map,
                         &depth_if_map,
                         p_err_info);
  if (ret == 0) {
    template_map_[template_name] = depth_template_map;
    template_combine_map_[template_name] = depth_combine_map;
    template_while_map_[template_name] = depth_while_map;
    template_until_map_[template_name] = depth_until_map;
    template_if_map_[template_name] = depth_if_map;
  }

  return ret;
}

int PlainExtractor::LoadTemplate(const string &template_name,
                                 const string &template_file,
                                 DepthPlainTemplateMap *p_depth_template_map,
                                 DepthTemplateCombineMap *p_depth_combine_map,
                                 DepthBlockIdxMap *p_depth_while_map,
                                 DepthBlockIdxMap *p_depth_until_map,
                                 DepthBlockIdxMap *p_depth_if_map,
                                 string *p_err_info) {
  char err_buf[kErrBufLen];
  vector<string> line_list;
  if (!GFile::LoadListFromFile(template_file, 0, &line_list)) {
    snprintf(err_buf, kErrBufLen, "load list from %s failed", template_file.c_str());
    *p_err_info = err_buf;
    WriteLog(kLogFatal, "%s", err_buf);
    return -1;
  }

  /// clear
  p_depth_template_map->clear();
  p_depth_combine_map->clear();
  p_depth_while_map->clear();
  p_depth_until_map->clear();
  p_depth_if_map->clear();

  /// whether the template is constructed successfully
  bool is_template_ok = true;
  size_t pos = string::npos;
  size_t end_pos = string::npos;
  int depth = 0;
  int line_no = 0;

  /// if is_template_ok==false, then break
  for (vector<string>::iterator it = line_list.begin();
      is_template_ok && it != line_list.end(); ++it) {
    const string &line = *it;
    line_no++;
    if (line.empty() || line[0] == '#')
      continue;

    /// calculate depth
    if (line[0] == '[') {
      end_pos = line.find(']');
      if (end_pos == string::npos) {
        snprintf(err_buf, kErrBufLen, "template[%s] depth line invalid", template_file.c_str());
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);
        is_template_ok = false;
        break;
      }
      string depth_str = line.substr(1, end_pos-1);
      depth = Text::StrToInt(depth_str);
      continue;
    }

    vector<string> item_list;
    Text::Segment(line, "\t", &item_list);
    if (item_list.size() < 1)
      continue;

    vector<PlainMatchLine> &match_line_list = (*p_depth_template_map)[depth];
    /// XXX add one new element
    match_line_list.resize(match_line_list.size() + 1);
    PlainMatchLine &match_line = match_line_list[match_line_list.size()-1];
    match_line.field_cmd = item_list[0];

    /// combine field
    vector<CombineLine> &combine_line_list = (*p_depth_combine_map)[depth];
    if (item_list[0] == "combine_field") {
      if (item_list.size() != 3) {
        WriteLog(kLogFatal, "load template[%s] invalid combine field:%s", template_name.c_str(), line.c_str());
      } else {
        CombineLine combine_field;
        combine_field.name = item_list[1];
        combine_field.value = item_list[2];
        combine_line_list.push_back(combine_field);
      }
      continue;
    }

    /// construct match line
    if (item_list.size() > 2) {
      match_line.capture_type = item_list[1];
      match_line.match_pattern = item_list[2];
      if (match_line.capture_type == kPlainCaptureTypeRegex) {
        string err_info;
        if (ConstructMatchRegex(match_line.match_pattern, &match_line, &err_info) < 0) {
          WriteLog(kLogWarning, "template[%s] depth[%d] failed[%s]", template_file.c_str(), depth, err_info.c_str());
          is_template_ok = false;
          break;
        }
      } else if (match_line.capture_type == kPlainCaptureTypeStr) {
        int number = 1;
        string name;
        size_t before_pos = 0;
        pos = item_list[2].find(kVarPrefix);
        while (pos != string::npos) {
          string prefix = item_list[2].substr(0, pos);
          end_pos = item_list[2].find(kVarSuffix, pos);
          name = item_list[2].substr(pos+2, end_pos-pos-2);
          VarCapture var_capture;
          var_capture.name = name;
          var_capture.number = number;
          number++;
          var_capture.pos = pos;
          match_line.var_capture_list.push_back(var_capture);
          before_pos = end_pos+2;
          pos = item_list[2].find(kVarPrefix, before_pos);
        }
      }
    }

    /// self-defined value
    if (item_list.size() > 3) {
      for (size_t i = 3; i< item_list.size(); i++) {
        string err_info;
        if (ConstructSelfDefinedVal(item_list[i], &match_line, &err_info) < 0) {
          snprintf(err_buf, kErrBufLen,
                   "template[%s] depth[%d] self-defined value failed",
                   template_file.c_str(),
                   depth);
          *p_err_info = string(err_buf) + err_info;
          WriteLog(kLogFatal, "%s", err_buf);
          is_template_ok = false;
          break;
        }
      }
    }
  }

  /// preprocess, get the start/end index for while/until/if block
  if (is_template_ok) {
    for (DepthPlainTemplateMap::const_iterator it = p_depth_template_map->begin();
        it != p_depth_template_map->end(); ++it) {
      int depth = it->first;
      const vector<PlainMatchLine> &match_line_list = it->second;
      BlockIdxMap while_map, until_map, if_map;
      string err_info;
      int ret = Preprocess(match_line_list, &while_map, &until_map, &if_map, &err_info);
      if (ret < 0) {
        snprintf(err_buf, kErrBufLen, "template[%s] depth[%d] preprocess failed:", template_file.c_str(), depth);
        *p_err_info = string(err_buf) + err_info;
        WriteLog(kLogFatal, "%s", err_buf);
        is_template_ok = false;
        break;
      }
      (*p_depth_while_map)[depth] = while_map;
      (*p_depth_until_map)[depth] = until_map;
      (*p_depth_if_map)[depth] = if_map;
    }
  }

  if (is_template_ok)
    return 0;
  return -1;
}

int PlainExtractor::Preprocess(const vector<PlainMatchLine> &match_line_list,
                               BlockIdxMap *p_while_map,
                               BlockIdxMap *p_until_map,
                               BlockIdxMap *p_if_map,
                               string *p_err_info) {
  char err_buf[kErrBufLen];
  /// store ctrl cmd, to check if cmd is matched
  stack<string> cmd_stack;
  bool is_valid = true;
  BlockIdxMap &while_map = *p_while_map;
  BlockIdxMap &until_map = *p_until_map;
  BlockIdxMap &if_map = *p_if_map;

  for (int i = 0; i < static_cast<int>(match_line_list.size()); i++) {
    const PlainMatchLine &match_line = match_line_list[i];
    const string &field_cmd = match_line.field_cmd;

    if (field_cmd == "while") {
      cmd_stack.push(field_cmd);
      /// find the position of `endwhile' for `while'
      int j = i+1;
      for (; j < static_cast<int>(match_line_list.size()); j++) {
        if (match_line_list[j].field_cmd == "endwhile") {
          break;
        }
      }
      if (j >= static_cast<int>(match_line_list.size()) || i+1 == j) {
        is_valid = false;
        snprintf(err_buf, kErrBufLen, "while/endwhile in conf error");
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);
        break;
      }
      while_map[i] = j;
    } else if (field_cmd == "endwhile") {
      /// check `while'
      if (cmd_stack.empty() || cmd_stack.top() != "while") {
        is_valid = false;
        snprintf(err_buf, kErrBufLen, "no matching while for endwhile:%d", i);
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);
        break;
      }
      cmd_stack.pop();
    } if (field_cmd == "until") {
      cmd_stack.push(field_cmd);
      /// find the position of `enduntil' for `until'
      int j = i+1;
      for (; j < static_cast<int>(match_line_list.size()); j++) {
        if (match_line_list[j].field_cmd == "enduntil") {
          break;
        }
      }
      if (j >= static_cast<int>(match_line_list.size()) || i+1 == j) {
        is_valid = false;
        snprintf(err_buf, kErrBufLen,"until/enduntilin conf error");
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);

        break;
      }
      until_map[i] = j;
    } else if (field_cmd == "enduntil") {
      /// check `until'
      if (cmd_stack.empty() || cmd_stack.top() != "until") {
        is_valid = false;
        snprintf(err_buf, kErrBufLen, "no matching until for enduntil:%d", i);
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);

        break;
      }
      cmd_stack.pop();
    } else if (field_cmd == "if") {
      cmd_stack.push(field_cmd);
      /// find the position of `endif' for `if'
      int j = i+1;
      for (; j < static_cast<int>(match_line_list.size()); j++) {
        if (match_line_list[j].field_cmd == "endif") {
          break;
        }
      }
      if (j >= static_cast<int>(match_line_list.size()) || i+1 == j) {
        is_valid = false;
        snprintf(err_buf, kErrBufLen, "no matching endif for if:%d", i);
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);

        break;
      }
      if_map[i] = j;
    } else if (field_cmd == "or") {
      /// check `if'
      if (cmd_stack.empty() || cmd_stack.top() != "if") {
        is_valid = false;
        snprintf(err_buf, kErrBufLen, "no matching if for or:%d", i);
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);

        break;
      }
    } else if (field_cmd == "endif") {
      /// check `if'
      if (cmd_stack.empty() || cmd_stack.top() != "if") {
        is_valid = false;
        snprintf(err_buf, kErrBufLen, "no matching if for endif:%d", i);
        *p_err_info = err_buf;
        WriteLog(kLogFatal, "%s", err_buf);

        break;
      }
      cmd_stack.pop();
    }
  }

  if (is_valid)
    return 0;
  else
    return -1;
}

int PlainExtractor::ExtractWrapper(const ExtractItem &extract_item,
                                   MatchedResultItem *p_matched_result_item) {
  p_matched_result_item->self_result.clear();
  p_matched_result_item->sub_result_list.clear();

  const string &template_name = extract_item.url_template;
  bool is_exists = true;
  int ret = 0;
  template_lock_.RdLock();
  if (template_map_.find(template_name) == template_map_.end()) {
    is_exists = false;
  } else {
    ret = Extract(extract_item, p_matched_result_item);
  }
  template_lock_.Unlock();
  if (is_exists)
    return ret;

  /// template non-exist, so load template and extract
  template_lock_.WrLock();
  do {
    ret = LoadTemplate(template_name, &(p_matched_result_item->err_info));
    if (ret < 0) {
      WriteLog(kLogFatal, "load template[%s] failed", template_name.c_str());
      break;
    }
    ret = Extract(extract_item, p_matched_result_item);
  } while (0);
  template_lock_.Unlock();

  return ret;
}

int PlainExtractor::Extract(const ExtractItem &extract_item,
                            MatchedResultItem *p_matched_result_item) {
  const string &url = extract_item.url;
  const string &template_name = extract_item.url_template;
  int depth = extract_item.depth;
  const string &body = extract_item.body;
  assert(p_matched_result_item);
  SelfResult *self_result = &p_matched_result_item->self_result;
  SubResultList *sub_result_list = &p_matched_result_item->sub_result_list;
  char err_buf[kErrBufLen];

  sub_result_list->clear();
  if (template_map_.find(template_name) == template_map_.end()) {
    snprintf(err_buf, kErrBufLen, "no template:%s", template_name.c_str());
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = err_buf;
    WriteLog(kLogNotice, "%s", err_buf);
    return -1;
  }
  if (template_map_[template_name].find(depth) == template_map_[template_name].end()) {
    snprintf(err_buf, kErrBufLen, "url:%s template:%s depth:%d not exist", url.c_str(), template_name.c_str(), depth);
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = err_buf;
    WriteLog(kLogNotice, "%s", err_buf);
    return -1;
  }
  const vector<PlainMatchLine> &match_line_list = template_map_[template_name][depth];
  /// start/end idx for the block
  BlockIdxMap &while_map = template_while_map_[template_name][depth];
  BlockIdxMap &until_map = template_until_map_[template_name][depth];
  BlockIdxMap &if_map = template_if_map_[template_name][depth];

  /// 当前块内的sep/field命令执行是否成功，如果某个匹配命令不在任何if/while块内且匹配失败，则整个匹配失败
  bool cmd_matched = true;
  /// body中待匹配的当前位置，值为string::npos表示上个sep/field命令执行失败
  size_t cur_body_pos = 0;
  SelfResult sub_result;

  /// 控制命令，仅入栈if/while
  stack<CtrlCmd> cmd_stack;

  /// 逐个执行匹配命令，允许if/while块内不匹配，如果不匹配则回到if/while上次开始匹配的位置进行下面的匹配，
  /// 即if/while块允许回退；if/while块外的命令如果不匹配，则匹配失败.
  /// 仅endwhile/endif可能把cmd_matched从false修改为true，仅sep/field可能修改cur_body_pos
  /// 变量i不能使用size_t，因为for循环中可能会对i设置为-1
  for (int i = 0; i < static_cast<int>(match_line_list.size()); i++) {
    const PlainMatchLine &match_line = match_line_list[i];
    const string &field_cmd = match_line.field_cmd;
    const string &capture_type = match_line.capture_type;
    const string &match_pattern = match_line.match_pattern;
    CtrlCmd cur_cmd;
    cur_cmd.cmd_name = field_cmd;

    if (field_cmd == "while") {
      sub_result.clear();
      cur_cmd.cmd_start = i;
      cur_cmd.cmd_exec_start_pos = cur_body_pos;
      assert(while_map.find(i) != while_map.end());
      /// 匹配的endwhile命令的位置
      cur_cmd.cmd_end = while_map[i];
      /// while命令入栈
      cmd_stack.push(cur_cmd);

      /// 与while块平级的sep/field匹配失败，因为while允许匹配不成功，所以移动i到while命令结束，即endwhile的下一个位置
      if (!cmd_matched) {
        /// XXX 此时i指向endwhile在match_line_list中的下标，下次循环i++后，i指向endwhile的下一个位置
        i = cur_cmd.cmd_end;
        cmd_stack.pop();
      }
    } else if (field_cmd == "endwhile") {
      CtrlCmd top_cmd = cmd_stack.top();
      /// 当前while块匹配失败，则回到当前while上次开始执行的位置，结束当前while块
      if (!cmd_matched) {
        cur_body_pos = top_cmd.cmd_exec_start_pos;
        cmd_matched = true;
      } else {
        /// 匹配成功，保存结果
        if (!sub_result.empty()) {
          sub_result_list->push_back(sub_result);
          sub_result.clear();
        }
        /// 继续回到当前while开始执行，直到执行失败，即!cmd_matched
        /// 回到开始命令的上一条，这样下次循环i++以后又开始执行当前while块
        i = top_cmd.cmd_start - 1;
      }
      cmd_stack.pop();
    } else if (field_cmd == "until") {
      /// 前面匹配失败，则跳过当前匹配
      if (!cmd_matched)
        break;

      /// 匹配的endutil命令的位置
      cur_cmd.cmd_end = until_map[i];

      size_t next_pos = string::npos;
      if (OnSep(match_line, body, cur_body_pos, &next_pos) < 0) {
        WriteLog(kLogDebug, "OnSep:[%s][%s][%s] %lu failed",
            field_cmd.c_str(),
            capture_type.c_str(),
            match_pattern.c_str(),
            cur_body_pos);
      } else {
        string until_body = body.substr(cur_body_pos, next_pos-cur_body_pos);
        string body_nocomment;
        ExtractorUtil::RemoveComment(until_body, &body_nocomment);
        /// construct field conf list for until block
        vector<PlainMatchLine> until_fieldconf_list;
        for (int idx = i+1; idx <= cur_cmd.cmd_end-1; idx++)
          until_fieldconf_list.push_back(match_line_list[idx]);

        /// construct if map for until block
        BlockIdxMap until_if_map;
        for (BlockIdxMap::iterator it_if = if_map.begin();
            it_if != if_map.end(); ++it_if) {
          int orig_start= it_if->first;
          int orig_end = it_if->second;
          until_if_map[orig_start-i-1] = orig_end-i-1;
        }

        SelfResult until_sub_result;
        OnUntil(body_nocomment, until_fieldconf_list, until_if_map, sub_result_list);

        cur_body_pos = next_pos;
      }

      /// 跳出until块
      /// XXX 此时i指向endwhile在match_line_list中的下标，下次循环i++后，i指向enduntil的下一个位置
      i = cur_cmd.cmd_end;
    } else {
      OnNonLoopField(match_line,
          if_map,
          body,
          &i,
          &cur_cmd,
          &cmd_stack,
          &cur_body_pos,
          &cmd_matched,
          self_result,
          &sub_result);
    }
  }

  /// 匹配失败
  if (!cmd_matched) {
    snprintf(err_buf, kErrBufLen, "url:%s template:%s depth:%d match failed", url.c_str(), template_name.c_str(), depth);
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = err_buf;
    WriteLog(kLogNotice, "%s", err_buf);
    return -1;
  }

  /// combine field
  const vector<CombineLine> &combine_list = template_combine_map_[template_name][depth];
  for (vector<CombineLine>::const_iterator it = combine_list.begin();
      it != combine_list.end(); ++it) {
    const string &name = it->name;
    const string &value = it->value;
    string filled_value;
    int ret = FillValue(value, *self_result, &filled_value);
    if (ret == 0) {
      (*self_result)[name].push_back(filled_value);
    }
  }

  if (sub_result_list->empty() && !sub_result.empty()) {
    sub_result_list->push_back(sub_result);
  }

  if (self_result->empty() && sub_result_list->empty()) {
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = "matched result empty. url[" + url + "] template[" + template_name + "]";
    WriteLog(kLogNotice, "url:%s template:%s depth:%d match failed:empty", url.c_str(), template_name.c_str(), depth);
    return -1;
  }

  p_matched_result_item->is_ok = true;
  WriteLog(kLogNotice, "url:%s template:%s depth:%d match OK", url.c_str(), template_name.c_str(), depth);
  return 0;
}

int PlainExtractor::OnUntil(const string &body, 
                            const vector<PlainMatchLine> &match_line_list,
                            const BlockIdxMap &if_map,
                            SubResultList *p_sub_result_list) {
  stack<CtrlCmd> cmd_stack;
  /// 当前块内的sep/field命令执行是否成功，如果某个匹配命令不在任何if/while块内且匹配失败，则整个匹配失败
  bool cmd_matched = true;
  /// body中待匹配的当前位置，值为string::npos表示上个sep/field命令执行失败
  size_t cur_body_pos = 0;

  while (cur_body_pos <= body.size()) {
    for (int i = 0; i < static_cast<int>(match_line_list.size()); i++) {
      const PlainMatchLine &match_line = match_line_list[i];
      const string &field_cmd = match_line.field_cmd;
      const string &capture_type = match_line.capture_type;
      const string &match_pattern = match_line.match_pattern;
      CtrlCmd cur_cmd;
      cur_cmd.cmd_name = field_cmd;

      if (field_cmd == "if") {
        cur_cmd.cmd_start = i;
        cur_cmd.cmd_exec_start_pos = cur_body_pos;
        BlockIdxMap::const_iterator it_if = if_map.find(i);
        assert(it_if != if_map.end());
        /// 查找匹配的endif命令的位置
        cur_cmd.cmd_end = it_if->second;
        /// if入栈
        cmd_stack.push(cur_cmd);

        /// 与if块平级的sep/field匹配失败，则结束if块，移动i到endif的下一个位置
        if (!cmd_matched) {
          i = cur_cmd.cmd_end;
          cmd_stack.pop();
        }
      } else if (field_cmd == "or") {
        /// 取出栈顶的if的开始执行位置，重新开始查找
        CtrlCmd top_cmd = cmd_stack.top();
        cur_body_pos = top_cmd.cmd_exec_start_pos;
        /// XXX trick 或的分支之间可认为平级的分支匹配成功
        cmd_matched = true;
      } else if (field_cmd == "endif") {
        /// 栈顶的if出栈
        CtrlCmd top_cmd = cmd_stack.top();
        cmd_stack.pop();
        /// 当前if块匹配失败，则结束if块，cur_body_pos回到当前if开始执行的位置
        if (!cmd_matched) {
          cur_body_pos = top_cmd.cmd_exec_start_pos;
          cmd_matched = true;
        }
      } else if (field_cmd == "sep") {
        /// 前面匹配失败，则跳过当前匹配
        if (!cmd_matched)
          break;
        size_t next_pos = string::npos;
        if (OnSep(match_line, body, cur_body_pos, &next_pos) < 0) {
          WriteLog(kLogDebug, "OnSep:[%s][%s][%s] %lu failed",
              field_cmd.c_str(),
              capture_type.c_str(),
              match_pattern.c_str(),
              cur_body_pos);
          cmd_matched = false;
        }
        cur_body_pos = next_pos;
      } else if (field_cmd == "field") {
        /// 前面匹配失败，则跳过当前匹配
        if (!cmd_matched)
          break;
        size_t next_pos = string::npos;
        SelfResult sub_result;
        if (OnField(match_line, body, cur_body_pos, &sub_result, &next_pos) < 0) {
          WriteLog(kLogDebug, "OnField:[%s][%s][%s] %lu failed",
              field_cmd.c_str(),
              capture_type.c_str(),
              match_pattern.c_str(),
              cur_body_pos);
          cmd_matched = false;
        } else {
          p_sub_result_list->push_back(sub_result);
        }
        cur_body_pos = next_pos;
      }
    }
  }

  return 0;
}

void PlainExtractor::OnNonLoopField(const PlainMatchLine &match_line,
                                    const BlockIdxMap &if_map,
                                    const string &body,
                                    int *p_i,
                                    CtrlCmd *p_cur_cmd,
                                    stack<CtrlCmd> *p_cmd_stack,
                                    size_t *p_cur_body_pos,
                                    bool *p_cmd_matched,
                                    SelfResult *self_result,
                                    SelfResult *p_sub_result) {
  const string &field_cmd = match_line.field_cmd;
  const string &capture_type = match_line.capture_type;
  const string &match_pattern = match_line.match_pattern;

  int &i = *p_i;
  CtrlCmd &cur_cmd = *p_cur_cmd;
  stack<CtrlCmd> &cmd_stack = *p_cmd_stack;
  size_t &cur_body_pos = *p_cur_body_pos;
  bool &cmd_matched = *p_cmd_matched;
  SelfResult &sub_result = *p_sub_result;

  do {
    if (field_cmd == "if") {
      cur_cmd.cmd_start = i;
      cur_cmd.cmd_exec_start_pos = cur_body_pos;
      BlockIdxMap::const_iterator it_if = if_map.find(i);
      assert(it_if != if_map.end());
      /// 查找匹配的endif命令的位置
      cur_cmd.cmd_end = it_if->second;
      /// if入栈
      cmd_stack.push(cur_cmd);

      /// 与if块平级的sep/field匹配失败，则结束if块，移动i到endif的下一个位置
      if (!cmd_matched) {
        i = cur_cmd.cmd_end;
        cmd_stack.pop();
      }
    } else if (field_cmd == "or") {
      /// 取出栈顶的if的开始执行位置，重新开始查找
      CtrlCmd top_cmd = cmd_stack.top();
      cur_body_pos = top_cmd.cmd_exec_start_pos;
      /// XXX trick 或的分支之间可认为平级的分支匹配成功
      cmd_matched = true;
    } else if (field_cmd == "endif") {
      /// 栈顶的if出栈
      CtrlCmd top_cmd = cmd_stack.top();
      cmd_stack.pop();
      /// 当前if块匹配失败，则结束if块，cur_body_pos回到当前if开始执行的位置
      if (!cmd_matched) {
        cur_body_pos = top_cmd.cmd_exec_start_pos;
        cmd_matched = true;
      }
    } else if (field_cmd == "sep") {
      /// 前面匹配失败，则跳过当前匹配
      if (!cmd_matched)
        break;
      size_t next_pos = string::npos;
      if (OnSep(match_line, body, cur_body_pos, &next_pos) < 0) {
        WriteLog(kLogDebug, "OnSep:[%s][%s][%s] %lu failed",
            field_cmd.c_str(),
            capture_type.c_str(),
            match_pattern.c_str(),
            cur_body_pos);
        cmd_matched = false;
      }
      cur_body_pos = next_pos;
    } else if (field_cmd == "field") {
      /// 前面匹配失败，则跳过当前匹配
      if (!cmd_matched)
        break;
      size_t next_pos = string::npos;
      if (OnField(match_line, body, cur_body_pos, &sub_result, &next_pos) < 0) {
        WriteLog(kLogDebug, "OnField:[%s][%s][%s] %lu failed",
            field_cmd.c_str(),
            capture_type.c_str(),
            match_pattern.c_str(),
            cur_body_pos);
        cmd_matched = false;
      }
      cur_body_pos = next_pos;
    } else if (field_cmd == "self_field") {
      /// 前面匹配失败，则跳过当前匹配
      if (!cmd_matched)
        break;
      size_t next_pos = string::npos;
      if (OnField(match_line, body, cur_body_pos, self_result, &next_pos) < 0) {
        WriteLog(kLogDebug, "OnSelfField:[%s][%s][%s] %lu failed",
            field_cmd.c_str(),
            capture_type.c_str(),
            match_pattern.c_str(),
            cur_body_pos);

        cmd_matched = false;
      }
      cur_body_pos = next_pos;
    }
  } while (0);
}

int PlainExtractor::OnSep(const PlainMatchLine &match_line, const string &body, size_t pos, size_t *next_pos) {
  const string &capture_type = match_line.capture_type;
  const string &match_pattern = match_line.match_pattern;
  if (capture_type == kPlainCaptureTypeStr) {
    *next_pos = body.find(match_pattern, pos);
    if (*next_pos == string::npos) {
      return -1;
    } else {
      *next_pos += match_pattern.size();
      return 0;
    }
  } else if (capture_type == kPlainCaptureTypeRegex) {
    smatch what;
    try {
      if (regex_search(body.begin()+pos, body.end(), what, match_line.match_regex)) {
        /// 成功匹配的下一个位置
        *next_pos = what[0].second - body.begin();
        return 0;
      } else {
        return -1;
      }
    } catch(const regex_error &e) {
      WriteLog(kLogWarning, "regex_search failed:%s", e.what());
      return -1;
    }
  }
  return -1;
}

int PlainExtractor::OnField(const PlainMatchLine &match_line,
                            const string &body,
                            size_t pos,
                            SelfResult *result_map,
                            size_t *next_pos) {
  assert(result_map != NULL);
  *next_pos = string::npos;

  size_t start_pos = 0;
  const string &capture_type = match_line.capture_type;
  const string &match_pattern = match_line.match_pattern;
  const vector<VarCapture> &var_capture_list = match_line.var_capture_list;
  const vector<SelfDefinedVal> &self_defined_val_list = match_line.self_defined_val_list;
  vector<string> sub_group_list;   /// save all subgroup of match

  if (var_capture_list.empty()) {
    return -1;
  }

  if (capture_type == kPlainCaptureTypeStr) {
    /// 第一个字段的前缀字符串，即第一个字段%>到第二个字段<%间的部分
    string prefix_str = match_pattern.substr(0, var_capture_list[0].pos);
    pos = body.find(prefix_str, pos);
    if (pos == string::npos) {
      return -1;
    }

    /// prefix str在body中出现的位置
    size_t prefix_pos = pos + prefix_str.size();
    SelfResult name_value_list;
    for (size_t i = 0; i < var_capture_list.size(); i++) {
      /// 配置字符串中的当前字段
      const VarCapture &var_capture = var_capture_list[i];

      size_t conf_str_pos = var_capture.pos + var_capture.name.size() + 4;
      /// 当前字段的后缀字符串，即当前字段%>到下一个字段<%间的部分
      string suffix_str;
      if (i == var_capture_list.size() -1)
        suffix_str = match_pattern.substr(conf_str_pos);
      else
        suffix_str = match_pattern.substr(conf_str_pos, var_capture_list[i+1].pos - conf_str_pos);
      /// suffix str在body中出现的位置
      size_t suffix_pos = body.find(suffix_str, prefix_pos);
      /// 根据prefix str和suffix str在body中找到字段对应的字符串
      if (suffix_pos == string::npos) {
        pos = string::npos;
        break;
      }
      pos = suffix_pos + suffix_str.size();
      string value = body.substr(prefix_pos, suffix_pos - prefix_pos);
      name_value_list[var_capture.name].push_back(value);
      sub_group_list.push_back(value);
      prefix_pos = pos;
    }

    if (pos == string::npos) {
      return -1;
    }

    /// 如果整个字符串匹配成功，则把结果保存到result_map中
    for (SelfResult::iterator it = name_value_list.begin();
        it != name_value_list.end(); ++it) {
      const string &name = it->first;
      vector<string> &value_list = it->second;
      if (result_map->find(name) == result_map->end()) {
        (*result_map)[name] = value_list;
      } else {
        copy(value_list.begin(), value_list.end(), back_inserter((*result_map)[name]));
      }
    }
    *next_pos = pos;
  } else if (capture_type == kPlainCaptureTypeRegex) {
    smatch what;
    int match_count = 0;
    try {
      string::const_iterator start_itr = body.begin()+pos;
      if (regex_search(start_itr, body.end(), what, match_line.match_regex)) {
        match_count = what.size();

        /// 捕获正则表达式命名/索引
        for (size_t i = 0; i < var_capture_list.size(); i++) {
          const VarCapture &var_capture = var_capture_list[i];
          string value = what[var_capture.number];
          (*result_map)[var_capture.name].push_back(value);
          sub_group_list.push_back(value);
        }

        *next_pos = what[0].second - body.begin();
      }
    } catch(const regex_error &e) {
      WriteLog(kLogWarning, "regex_search error:%s", e.what());
    }
    if (match_count == 0)
      return -1;
  }

  /// self-defined value
  if (!sub_group_list.empty()) {
    for (size_t i = 0; i < self_defined_val_list.size(); i++) {
      const SelfDefinedVal &self_defined_val = self_defined_val_list[i];
      string value;
      start_pos = 0;
      vector<pair<int, size_t> >::const_iterator iter = self_defined_val.number_pos_list.begin();
      for (; iter != self_defined_val.number_pos_list.end(); iter++) {
        int group_no = iter->first;
        size_t group_pos = iter->second;
        const string &replace_str = sub_group_list[group_no-1];
        value += self_defined_val.value.substr(start_pos, group_pos - start_pos) + replace_str;
        start_pos = group_pos + 2;
      }
      value += self_defined_val.value.substr(start_pos);
      (*result_map)[self_defined_val.name].push_back(value);
    }
  }

  return 0;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor
