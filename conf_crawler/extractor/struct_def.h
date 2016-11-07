/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_extractor/struct_def.h
 * @namespace ganji::crawler::conf_extractor
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_STRUCT_DEF_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_STRUCT_DEF_H_

#include <boost/regex.hpp>

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <map>

#include "conf_crawler/common/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace extractor {
const char kPlainPath[] = "plain";
const char kCssSelectorPath[] = "css_selector";
/// domain.category_\d+\.txt
const char kMultipleTemplateSuffix[] = "_\\d+\\.txt";
const char kTemplateSuffix[] = "\\.txt";
/// delimeter between domain name and template
/// i.e. domain_name.xxxx
const char kTemplateDomainDelim[] = ".";

const int kErrBufLen = 1024;

/// prefix for variable capture
const char kVarPrefix[] = "<%";
/// suffix for variable capture
const char kVarSuffix[] = "%>";

/// match type for css template
const char kCssMatchTypeOptional[] = "optional";
const char kCssMatchTypeMandatory[] = "mandatory";
const char kCssMatchTypeMultiple[] = "multiple";
/// match target for css template
const char kCssMatchTargetAttribute[] = "attribute";
const char kCssMatchTargetPlainText[] = "plain_text";
const char kCssMatchTargetHtml[] = "html";
const char kCssMatchTargetText[] = "text";

/**
 * @struct CtrlCmd
 * @desc control command to process html body
 */
struct CtrlCmd {
  std::string cmd_name;             ///< the name of cmd
  int cmd_start;                    ///< the start pos of cmd in vector<PlainMatchLine>
  int cmd_end;                      ///< the end pos of cmd in vector<PlainMatchLine>
  size_t cmd_exec_start_pos;        ///< the pos in which cmd started executing in web page
  size_t cmd_exec_good_end_pos;
};

/**
 * @struct VarCapture
 * @desc The variable captures in match_pattern
 */
struct VarCapture {
  std::string name;                  ///< the captured variable name
  int number;                        ///< the number_th sub express, the number_th "("
  size_t pos;                        ///< the pos in MatchLine.match_pattern
};

/**
 * @struct SelfDefinedVal
 * @desc combination of values for captured variables
 */
struct SelfDefinedVal {
  std::string name;                                         ///< the name of defined value
  std::string value;                                   ///< the value for self-defined variable
  std::vector<std::pair<int, size_t> > number_pos_list;     ///< the list of captured variable no. and pos in value
};

/**
 * @struct BaseMatchLine
 * @desc base match line, corresponding to each line in configure file
 */
struct BaseMatchLine {
  std::string match_pattern;       ///< match pattern used to extract
  boost::regex match_regex;                           ///< regex for match pattern
  std::vector<VarCapture> var_capture_list;            ///< variable capture list
  std::vector<SelfDefinedVal> self_defined_val_list;   ///< self-defined value list
};

/// capture type for plain template
const char kPlainCaptureTypeStr[] = "str";
const char kPlainCaptureTypeRegex[] = "regex";

/**
 * @struct PlainMatchLine
 * @desc one line in config file
 */
struct PlainMatchLine: public BaseMatchLine {
  std::string field_cmd;           /// including ctrl cmd and match cmd
  std::string capture_type;        /// range in kPlainCaptureType*
};

/**
 * @struct CssMatchLine
 * @desc one line in css config file
 */
struct CssMatchLine: public BaseMatchLine {
  std::string match_type;          ///< range in kCssMatchType
  std::string selector_str;        ///< css selector
  std::string match_target;        ///< range in kCssMatchTarget
  std::string match_attr;          ///< attribute to match, for `attribute' only
  std::string attr_var;            ///< attr variable name to save, for `attribute' && `plain_text' only
};

/// depth => each line in template.conf
typedef std::map<int, std::vector<PlainMatchLine> > DepthPlainTemplateMap;
/// template name => DepthPlainTemplateMap
typedef std::map<std::string, DepthPlainTemplateMap> PlainTemplateMap;

/// start idx => end idx  for control(while/until/if) line block
typedef std::map<int, int> BlockIdxMap;
/// depth => BlockIdxMap
typedef std::map<int, BlockIdxMap> DepthBlockIdxMap;
/// template name => DepthBlockIdxMap
typedef std::map<std::string, DepthBlockIdxMap> TemplateBlockIdxMap;

/// depth => each line in template.conf
typedef std::map<int, std::vector<CssMatchLine> > DepthCssTemplateMap;
/// template name => list of full_template_name
typedef std::map<std::string, std::list<std::string> > CssTemplateNameMap;
/// template name => list of DepthCssTemplateMap
typedef std::map<std::string, std::list<DepthCssTemplateMap> > CssTemplateMap;

/**
 * @struct CombineLine
 * @desc combination of self_fields
 */
struct CombineLine {
  std::string name;                  ///< the name of combined field
  std::string value;                 ///< the value of combined field
};

/// depth => each combine field in template.conf
typedef std::map<int, std::vector<CombineLine> > DepthTemplateCombineMap;
/// template name => list of DepthTemplateCombineMap
typedef std::map<std::string, std::list<DepthTemplateCombineMap> > TemplateCombineListMap;
typedef std::map<std::string, DepthTemplateCombineMap> TemplateCombineMap;

typedef std::map<std::string, std::vector<std::string> > SelfResult;
typedef std::vector<SelfResult> SubResultList;
}}}};

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_EXTRACTOR_STRUCT_DEF_H_
