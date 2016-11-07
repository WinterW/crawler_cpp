/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/css_extractor.h
 * @namespace ganji::crawler::conf_extractor::extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2012-04-09
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "conf_crawler/extractor/css_extractor.h"

#include <assert.h>
#include <stack>
#include <utility>
#include <algorithm>

#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/utils.h>

#include "util/text/text.h"
#include "util/file/file.h"
#include "util/time/time.h"
#include "util/log/thread_fast_log.h"

#include "conf_crawler/extractor/extractor_config.h"
#include "conf_crawler/extractor/extractor_util.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
using std::string;
using std::vector;
using std::list;
using std::map;
using std::pair;

using boost::smatch;
using boost::regex;
using boost::regex_search;
using boost::regex_error;

using htmlcxx::HTML::ParserDom;
using htmlcxx::HTML::Node;
using htmlcxx::HTML::decode_entities;
using hcxselect::Selector;
using hcxselect::ParseException;

namespace GFile = ::ganji::util::file;
namespace Text = ::ganji::util::text::Text;
namespace Time = ::ganji::util::time;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

int CssExtractor::LoadTemplate(const string &template_name, string *p_err_info) {
  string domain, category;
  if (ParseTemplateName(template_name, &domain, &category) < 0) {
    WriteLog(kLogFatal, "invalid template[%s], no domain", template_name.c_str());
    return -1;
  }
  string template_path = p_config_->TemplatePath() + "/" +
                         string(kCssSelectorPath) + "/" +
                         domain + "/";
  vector<string> file_list;
  list<DepthCssTemplateMap> depth_template_list;
  list<DepthTemplateCombineMap> depth_combine_list;
  list<string> full_template_name_list;
  /// scan template path to find the template files
  int ret = GetMultipleTemplates(template_path, template_name, &file_list);
  if (ret < 0) {
    *p_err_info = "GetMultipleTemplates[" + template_path + "][" + template_name + "] failed";
    WriteLog(kLogFatal, p_err_info->c_str());
    return -1;
  }

  for (vector<string>::iterator it = file_list.begin();
      it != file_list.end(); ++it) {
    const string &template_name = *it;
    string template_file = template_path + "/" + template_name;
    DepthCssTemplateMap depth_template_map;
    DepthTemplateCombineMap depth_combine_map;
    string err_info;
    int ret = LoadTemplate(template_name,
                           template_file,
                           &depth_template_map,
                           &depth_combine_map,
                           &err_info);
    if (ret == 0) {
      depth_template_list.push_back(depth_template_map);
      depth_combine_list.push_back(depth_combine_map);
      full_template_name_list.push_back(template_name);
    } else {
      *p_err_info += "load_template[" + template_file + "] failed:" + err_info;
    }
  }
  if (depth_template_list.empty()) {
    WriteLog(kLogFatal, "LoadTemplate[%s] failed", template_name.c_str());
    return -1;
  }

  list<DepthCssTemplateMap> &orig_template_list = template_map_[template_name];
  list<DepthTemplateCombineMap> &orig_combine_list = template_combine_map_[template_name];
  list<string> &orig_template_name_list = template_name_map_[template_name];

  orig_template_list.clear();
  copy(depth_template_list.begin(), depth_template_list.end(), back_inserter(orig_template_list));

  orig_combine_list.clear();
  copy(depth_combine_list.begin(), depth_combine_list.end(), back_inserter(orig_combine_list));

  orig_template_name_list.clear();
  copy(full_template_name_list.begin(), full_template_name_list.end(), back_inserter(orig_template_name_list));

  WriteLog(kLogNotice, "LoadTemplate[%s] #template:%lu OK", template_name.c_str(), orig_template_list.size());

  return 0;
}

int CssExtractor::LoadTemplate(const string &template_name,
                               const string &template_file,
                               DepthCssTemplateMap *p_depth_template_map,
                               DepthTemplateCombineMap *p_depth_combine_map,
                               string *p_err_info) {
  char err_buf[kErrBufLen];

  vector<string> line_list;
  if (!GFile::LoadListFromFile(template_file, 0, &line_list)) {
    WriteLog(kLogWarning, "load %s failed", template_file.c_str());
    return -1;
  }

  /// clear
  p_depth_template_map->clear();
  p_depth_combine_map->clear();

  /// whether template is constructed correctly
  bool is_template_ok = true;

  int depth = 0;
  /// if is_template_ok==false, then break
  for (vector<string>::const_iterator it = line_list.begin();
      is_template_ok && it != line_list.end(); ++it) {
    const string &line = *it;
    if (line.empty() || line[0] == '#')
      continue;

    /// parse depth
    if (line[0] == '[') {
      size_t pos = line.find(']');
      string depth_str = line.substr(1, pos-1);
      depth = Text::StrToInt(depth_str);
      (*p_depth_template_map)[depth].clear();
      continue;
    }

    vector<string> item_list;
    Text::Segment(line, "\t", &item_list);
    if (item_list.size() < 1)
      continue;

    vector<CssMatchLine> &match_line_list = (*p_depth_template_map)[depth];
    /// XXX add one new element
    match_line_list.resize(match_line_list.size() + 1);
    CssMatchLine &match_line = match_line_list[match_line_list.size() -1];
    match_line.match_type = item_list[0];

    /// construct combine field
    if (item_list[0] == "combine_field") {
      if (item_list.size() != 3) {
        WriteLog(kLogWarning, "template[%s] depth[%d] invalid combine field:%s", template_file.c_str(), depth, line.c_str());
      } else {
        vector<CombineLine> &combine_line_list = (*p_depth_combine_map)[depth];
        CombineLine combine_field;
        combine_field.name = item_list[1];
        combine_field.value = item_list[2];
        combine_line_list.push_back(combine_field);
      }
      continue;
    }

    if (item_list.size() < 4) {
      WriteLog(kLogWarning, "template[%s] depth[%d] #field<4:%s", template_file.c_str(), depth, line.c_str());
      is_template_ok = false;
      break;
    }

    /// match type
    match_line.match_type = item_list[0];
    if (match_line.match_type != kCssMatchTypeOptional &&
        match_line.match_type != kCssMatchTypeMandatory &&
        match_line.match_type != kCssMatchTypeMultiple) {
      WriteLog(kLogWarning, "template[%s] depth[%d] invalid match_type:%s", template_file.c_str(), depth, line.c_str());
      is_template_ok = false;
      break;
    }

    match_line.selector_str = item_list[1];
    match_line.match_target = item_list[2];
    /// attribute
    if (match_line.match_target == kCssMatchTargetAttribute) {
      if (item_list.size() < 5 || item_list[3].empty() || item_list[4].empty()) {
        WriteLog(kLogWarning, "template[%s] depth[%d] invalid attribute line:%s", template_file.c_str(), depth, line.c_str());
      } else {
        match_line.match_attr = item_list[3];
        match_line.attr_var = item_list[4];
      }
      continue;
    }
    /// plain_text
    if (match_line.match_target == kCssMatchTargetPlainText) {
      if (item_list.size() < 4 || item_list[3].empty()) {
        WriteLog(kLogWarning, "template[%s] depth[%d] invalid attribute line:%s", template_file.c_str(), depth, line.c_str());
      } else {
        match_line.attr_var = item_list[3];
      }
      continue;
    }

    /// html/text
    if (match_line.match_target != kCssMatchTargetHtml &&
        match_line.match_target != kCssMatchTargetText) {
      WriteLog(kLogWarning, "template[%s] depth[%d] invalid match_target:%s", template_file.c_str(), depth, line.c_str());
      is_template_ok = false;
      break;
    }
    match_line.match_pattern = item_list[3];

    /// construct regex
    string err_info;
    if (ConstructMatchRegex(match_line.match_pattern, &match_line, &err_info) < 0) {
      WriteLog(kLogWarning, "template[%s] depth[%d] failed[%s]", template_file.c_str(), depth, err_info.c_str());
      is_template_ok = false;
      break;
    }

    /// self-defined value
    if (item_list.size() > 4) {
      for (size_t i = 4; i< item_list.size(); i++) {
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

  if (!is_template_ok) {
    return -1;
  }

  return 0;
}

int CssExtractor::ExtractWrapper(const ExtractItem &extract_item,
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

int CssExtractor::Extract(const ExtractItem &extract_item,
                          MatchedResultItem *p_matched_result_item) {
  const string &url = extract_item.url;
  const string &template_name = extract_item.url_template;
  assert(p_matched_result_item);
  char err_buf[kErrBufLen];

  if (template_map_.find(template_name) == template_map_.end()) {
    snprintf(err_buf, kErrBufLen, "no template[%s] for url[%s]", template_name.c_str(), url.c_str());
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = err_buf;
    WriteLog(kLogNotice, "%s", err_buf);
    return -1;
  }

  const list<DepthCssTemplateMap> &depth_template_list = template_map_[template_name];
  assert(!depth_template_list.empty());
  const list<DepthTemplateCombineMap> &depth_combine_list = template_combine_map_[template_name];
  assert(!depth_template_list.empty());
  const list<string> &full_template_name_list = template_name_map_[template_name];
  assert(!full_template_name_list.empty());

  /// multiple templates
  list<DepthCssTemplateMap>::const_iterator it_template = depth_template_list.begin();
  list<DepthTemplateCombineMap>::const_iterator it_combine = depth_combine_list.begin();
  list<string>::const_iterator it_name = full_template_name_list.begin();
  list<MatchedResultItem> matched_result_list;
  list<string> matched_template_name_list;
  string err_info;
  for (; it_template != depth_template_list.end(); ++it_template, ++it_combine, ++it_name) {
    const DepthCssTemplateMap &depth_template_map = *it_template;
    const DepthTemplateCombineMap &depth_combine_map = *it_combine;
    const string &full_template_name = *it_name;
    MatchedResultItem matched_result_item;
    int ret = Extract(full_template_name, depth_template_map, depth_combine_map, extract_item, &matched_result_item);
    if (ret == 0) {
      matched_result_list.push_back(matched_result_item);
      matched_template_name_list.push_back(full_template_name);
    } else {
      err_info += matched_result_item.err_info + " ";
    }
  }

  if (matched_result_list.empty()) {
    p_matched_result_item->err_info = err_info;
    WriteLog(kLogWarning, "Extract[%s] by template[%s] failed:%s", url.c_str(), template_name.c_str(), err_info.c_str());
    return -1;
  }

  /// simple policy: select matched result with the most extracted items
  int max_count = 0;
  const MatchedResultItem *p_max_item = NULL;
  string matched_template_name;
  list<string>::const_iterator it_matched_name = matched_template_name_list.begin();
  for (list<MatchedResultItem>::iterator it = matched_result_list.begin();
      it != matched_result_list.end(); ++it, ++it_matched_name) {
    const MatchedResultItem &result_item = *it;
    int matched_item_count = result_item.self_result.size() + result_item.sub_result_list.size();
    if (matched_item_count > max_count) {
      p_max_item = &result_item;
      matched_template_name = *it_matched_name;
      max_count = matched_item_count;
    }
  }
  if (p_max_item) {
    *p_matched_result_item = *p_max_item;
    WriteLog(kLogNotice, "Extract[%s] by template[%s] OK", url.c_str(), matched_template_name.c_str());
    return 0;
  }

  return -1;
}

int CssExtractor::Extract(const string &full_template_name,
                          const DepthCssTemplateMap &depth_template_map,
                          const DepthTemplateCombineMap &depth_combine_map,
                          const ExtractItem &extract_item,
                          MatchedResultItem *p_matched_result_item) {
  const string &url = extract_item.url;
  int depth = extract_item.depth;
  /// XXX decode html entities
  string body = decode_entities(extract_item.body);
  //modified by wangshijie
  if (full_template_name.find("58") !=  full_template_name.npos) {
    body = "<html>\n<body>\n" + body;
  }
  //end
  assert(p_matched_result_item);
  SelfResult &self_result = p_matched_result_item->self_result;
  SubResultList &sub_result_list = p_matched_result_item->sub_result_list;
  p_matched_result_item->is_ok = true;
  char err_buf[kErrBufLen];

  DepthCssTemplateMap::const_iterator it_template = depth_template_map.find(depth);
  if (it_template == depth_template_map.end()) {
    snprintf(err_buf, kErrBufLen, "url[%s] template[%s] depth[%d] not exist", url.c_str(), full_template_name.c_str(), depth);
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = err_buf;
    WriteLog(kLogNotice, "%s", err_buf);
    return -1;
  }

  const vector<CssMatchLine> &match_line_list = it_template->second;

  /// parse body html
  ParserDom parser;
  tree<Node> dom = parser.parseTree(body);
  Selector root(dom);

  /// extract each match line sequentially
  for (int i = 0; i < static_cast<int>(match_line_list.size()); i++) {
    const CssMatchLine &match_line = match_line_list[i];
    int ret = ExtractByMatchLine(full_template_name,
                                 extract_item,
                                 body,
                                 match_line,
                                 root,
                                 p_matched_result_item);
    if (ret < 0) {
      p_matched_result_item->is_ok = false;
      break;
    }
  }

  /// combine field
  DepthTemplateCombineMap::const_iterator it_combine = depth_combine_map.find(depth);
  if (it_combine != depth_combine_map.end()) {
    const vector<CombineLine> &combine_line_list = it_combine->second;
    for (vector<CombineLine>::const_iterator it = combine_line_list.begin();
        it != combine_line_list.end(); ++it) {
      const string &name = it->name;
      const string &value = it->value;
      string filled_value;
      int ret = FillValue(value, self_result, &filled_value);
      if (ret == 0) {
        self_result[name].push_back(filled_value);
      }
    }
  }

  if (!p_matched_result_item->is_ok) {
    p_matched_result_item->err_info = "matched failed. url[" + url + "] template[" + full_template_name + "]";
    WriteLog(kLogNotice, "url[%s] template[%s] depth[%d] match failed", url.c_str(), full_template_name.c_str(), depth);
    return -1;
  } else if (self_result.empty() && sub_result_list.empty()) {
    p_matched_result_item->is_ok = false;
    p_matched_result_item->err_info = "matched result empty. url[" + url + "] template[" + full_template_name + "]";
    WriteLog(kLogNotice, "url[%s] template[%s] depth[%d] match failed:empty", url.c_str(), full_template_name.c_str(), depth);
    return -1;
  }

  WriteLog(kLogNotice, "url[%s] template[%s] depth[%d] match OK", url.c_str(), full_template_name.c_str(), depth);
  return 0;
}

int CssExtractor::ExtractByMatchLine(const string &full_template_name,
                                     const ExtractItem &extract_item,
                                     const string &body,
                                     const CssMatchLine &match_line,
                                     Selector &root,
                                     MatchedResultItem *p_matched_result_item) {
  const string &url = extract_item.url;
  int depth = extract_item.depth;

  const string &match_type = match_line.match_type;
  const string &selector_str = match_line.selector_str;
  const string &match_target = match_line.match_target;
  const string &match_attr = match_line.match_attr;
  const string &attr_var = match_line.attr_var;
  const string &match_pattern = match_line.match_pattern;

  SelfResult &self_result = p_matched_result_item->self_result;
  SubResultList &sub_result_list = p_matched_result_item->sub_result_list;

  /// get candidate nodes by css selector
  Selector s;
  int ret = GetNodesBySelector(selector_str, root, &s);
  if (ret < 0) {
    return -1;
  }

  if (s.empty()) {
    int event = kLogDebug;
    int ret = 0;
    /// empty nodes do not satify mandatory type
    if (match_type == kCssMatchTypeMandatory) {
      event = kLogWarning;
      ret = -1;
    }
    WriteLog(event, "url[%s] template[%s] depth[%d] select()[%s] empty", 
             url.c_str(),
             full_template_name.c_str(),
             depth,
             selector_str.c_str());
    return ret;
  }

  /// match with attribute
  if (match_target == kCssMatchTargetAttribute) {
    list<string> attr_value_list;
    int ret = GetNodesAttr(s, match_attr, &attr_value_list);
    if (ret < 0) {
      if (match_type == kCssMatchTypeMandatory) {
        WriteLog(kLogWarning, "url[%s] template[%s] depth[%d] select()[%s] attr[%s] empty", 
                 url.c_str(),
                 full_template_name.c_str(),
                 depth,
                 selector_str.c_str(),
                 match_attr.c_str());
        return -1;
      }
    } else if (match_type == kCssMatchTypeMultiple) {
      for (list<string>::const_iterator it = attr_value_list.begin();
          it != attr_value_list.end(); ++it) {
        SelfResult sub_map;
        sub_map[attr_var].push_back(*it);
        sub_result_list.push_back(sub_map);
      }
    } else {
      int value_count = attr_value_list.size();
      if (attr_value_list.size() > 1) {
        WriteLog(kLogWarning, "url[%s] template[%s] depth[%d] select()[%s] attr[%s] size:%d>1", 
                 url.c_str(),
                 full_template_name.c_str(),
                 depth,
                 selector_str.c_str(),
                 match_attr.c_str(),
                 value_count);
        // return -1;
      }
      const string &value = *(attr_value_list.begin());
      self_result[attr_var].push_back(value);
    }
    return 0;
  }
  
  /// match with html/text
  string sub_body;
  /*
  size_t start_pos = (*s.begin())->data.offset();
  size_t end_pos = (*s.rbegin())->data.offset() + (*s.rbegin())->data.length();
  sub_body = body.substr(start_pos, end_pos - start_pos);
  
  /// use text
  if (match_target == kCssMatchTargetText) {
    string plain_text;
    ExtractorUtil::ToPlainText(sub_body, &plain_text);
    sub_body = plain_text;
  }
  */

  /// use plain_text
  if (match_target == kCssMatchTargetPlainText) {
    size_t start_pos = (*s.begin())->data.offset();
    size_t end_pos = (*s.rbegin())->data.offset() + (*s.rbegin())->data.length();
    sub_body = body.substr(start_pos, end_pos - start_pos);
    Text::ReplaceStrStr("\n", "", &sub_body);
    string plain_text;
    ExtractorUtil::ToPlainText(sub_body, &plain_text);
    self_result[attr_var].push_back(plain_text);

    return 0;
  }

  
  SubResultList regex_sub_result;
  sub_body = "";
  int i = 0;
  for (Selector::const_iterator it = s.begin(); it != s.end(); ++it) {
    Node &node = (*it)->data;
    size_t last_pos = node.offset();
    size_t next_pos = node.length();
    sub_body =  body.substr(last_pos,next_pos);
    Text::ReplaceStrStr("\n", "", &sub_body);
    /// use text
    if (match_target == kCssMatchTargetText) {
      string plain_text;
      ExtractorUtil::ToPlainText(sub_body, &plain_text);
      sub_body = plain_text;
    }
    ret = MatchLineWithRegex(match_line, sub_body, &regex_sub_result);
  }
  
  //ret = MatchLineWithRegex(match_line, sub_body, &regex_sub_result);
  if (ret < 0) {
    if (match_type == kCssMatchTypeMandatory) {
      WriteLog(kLogWarning, "SubExtract url[%s] template[%s] depth[%d] MatchLineWithRegex()[%s] failed #regex_sub_result:%lu", 
               url.c_str(),
               full_template_name.c_str(),
               depth,
               match_pattern.c_str(),
               regex_sub_result.size());
      return -1;
    }
    return 0;
  }

  if (match_type == kCssMatchTypeMultiple) {
    copy(regex_sub_result.begin(), regex_sub_result.end(), back_inserter(sub_result_list));
  } else {
    SelfResult &result_map = regex_sub_result[0];
    for (SelfResult::iterator it = result_map.begin();
        it != result_map.end(); ++it) {
      const string &result_key = it->first;
      const vector<string> &result_value = it->second;
      copy(result_value.begin(), result_value.end(), back_inserter(self_result[result_key]));
    }
  }

  return 0;
}

int CssExtractor::GetNodesBySelector(const string &selector_str, Selector &root, Selector *p_s) {
  try {
    *p_s = root.select(selector_str);
    return 0;
  } catch(const ParseException &ex) {
    WriteLog(kLogWarning, "select() error[%s]:[%s] pos[%d]", selector_str.c_str(), ex.what(), ex.position());
    return -1;
  } catch(...) {
    WriteLog(kLogWarning, "select() error[%s]", selector_str.c_str());
    return -1;
  }
}

int CssExtractor::GetNodesAttr(const Selector &s, const string &match_attr, list<string> *p_list) {
  p_list->clear();

  for (Selector::const_iterator it = s.begin(); it != s.end(); ++it) {
    Node &node = (*it)->data;
    node.parseAttributes();
    pair<bool, string> attr_pair = node.attribute(match_attr);
    if (attr_pair.first) {
      p_list->push_back(attr_pair.second);
    }
  }

  if (p_list->empty())
    return -1;
  else
    return 0;
}

int CssExtractor::MatchLineWithRegex(const CssMatchLine &match_line,
                                     const string &body,
                                     SubResultList *p_sub_result_list) {
  const string &match_pattern = match_line.match_pattern;
  const vector<VarCapture> &var_capture_list = match_line.var_capture_list;
  const vector<SelfDefinedVal> &self_defined_val_list = match_line.self_defined_val_list;

  if (var_capture_list.empty()) {
    return -1;
  }

  smatch what;
  try {
    string::const_iterator start_itr = body.begin();
    /// capture repeatedly
    while (regex_search(start_itr, body.end(), what, match_line.match_regex)) {
      SelfResult result_map;
      vector<string> subgroup_list;   ///< save all subgroup of match

      for (size_t i = 0; i < var_capture_list.size(); i++) {
        const VarCapture &var_capture = var_capture_list[i];
        string value = what[var_capture.number];
        result_map[var_capture.name].push_back(value);
        subgroup_list.push_back(value);
      }

      /// self-defined values
      if (!subgroup_list.empty()) {
        for (size_t i = 0; i < self_defined_val_list.size(); i++) {
          const SelfDefinedVal &self_defined_val = self_defined_val_list[i];
          string value;
          size_t start_pos = 0;
          vector<pair<int, size_t> >::const_iterator iter = self_defined_val.number_pos_list.begin();
          for (; iter != self_defined_val.number_pos_list.end(); iter++) {
            int group_no = iter->first;
            size_t group_pos = iter->second;
            const string &replace_str = subgroup_list[group_no-1];
            value += self_defined_val.value.substr(start_pos, group_pos - start_pos) + replace_str;
            start_pos = group_pos + 2;
          }
          value += self_defined_val.value.substr(start_pos);
          result_map[self_defined_val.name].push_back(value);
        }
      }

      start_itr = what[0].second;

      p_sub_result_list->push_back(result_map);
    }
  } catch(const regex_error &e) {
    WriteLog(kLogWarning, "regex_search[%s] regex error:%s", match_pattern.c_str(), e.what());
  }
  catch(std::runtime_error e){
    WriteLog(kLogWarning, "regex_search[%s] runtime error:%s", match_pattern.c_str(), e.what());
  }
  catch(std::bad_alloc e){
    WriteLog(kLogWarning, "regex_search[%s] bad_alloc error:%s", match_pattern.c_str(), e.what());
  }

  if (p_sub_result_list->empty())
    return -1;
  return 0;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::extractor

