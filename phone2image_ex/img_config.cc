/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/img_config.cpp
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */
#include <sys/types.h>
#include <unistd.h>
#include "phone2image_ex/img_config.h"

#include <string.h>
#include <math.h>

#include <string>
#include <map>

#include "util/log/thread_fast_log.h"
#include "util/text/text.h"
#include "util/file/file.h"
#include "util/xml/tinystr.h"
#include "util/xml/tinyxml.h"

namespace ganji { namespace crawler { namespace phone2image {
using std::map;
using std::string;
using std::vector;
using ganji::util::thread::Mutex;
namespace Text = ::ganji::util::text::Text;
namespace GFile = ::ganji::util::file;
namespace FastLog = ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogNotice;

int ImgConfig::Init(const string &conf_path) {
  int ret = LoadConf(conf_path);

  return ret;
}

int ImgConfig::LoadConf(const string &conf_path) {
  string conf_file = conf_path + "/" + string(kConfFile);
  if (!GFile::IsFileExist(conf_file)) {
    WriteLog(kLogFatal, "no conf file:%s", conf_file.c_str());
    return -1;
  }

  TiXmlDocument my_document(conf_file.c_str());
  my_document.LoadFile();

  TiXmlElement *root_element = my_document.RootElement();
  if (!root_element) {
    WriteLog(kLogFatal, "no child node in xml");
    return -1;
  }

  TiXmlElement* thread_num = root_element->FirstChildElement("thread_num");
  if (!thread_num) {
    WriteLog(kLogFatal, "thread_num not set");
    return -1;
  }
  thread_num_ = atoi(thread_num->FirstChild()->Value());

  TiXmlElement *rand_font_level = root_element->FirstChildElement("rand_font_level");
  if (!rand_font_level) {
    WriteLog(kLogFatal, "rand_font_level not set");
    return -1;
  }
  rand_font_level_ = atoi(rand_font_level->FirstChild()->Value());

  TiXmlElement *url_prefix = root_element->FirstChildElement("url_prefix");
  if (!url_prefix) {
    WriteLog(kLogFatal, "url_prefix not set");
    return -1;
  }
  url_prefix_ = url_prefix->FirstChild()->Value();

  TiXmlElement *check_domain = root_element->FirstChildElement("check_domain");
  if (!check_domain) {
    WriteLog(kLogFatal, "check_domain not set");
    return -1;
  }
  /// 仅值为1对应true
  string check_domain_str = check_domain->FirstChild()->Value();
  if (check_domain_str == "1")
    check_domain_ = true;
  else
    check_domain_ = false;

  TiXmlElement *exclude_inner_ip = root_element->FirstChildElement("exclude_inner_ip");
  if (!exclude_inner_ip) {
    WriteLog(kLogFatal, "exclude_inner_ip not set");
    return -1;
  }
  /// 仅值为1对应true
  string exclude_inner_ip_str = exclude_inner_ip->FirstChild()->Value();
  if (exclude_inner_ip_str == "1")
    exclude_inner_ip_ = true;
  else
    exclude_inner_ip_ = false;

  TiXmlElement *de_style = root_element->FirstChildElement("default_style");
  string _default_style = kDefaultStyle;
  if (de_style)
  {
    _default_style = de_style->FirstChild()->Value();
  }

  TiXmlElement *de_level = root_element->FirstChildElement("default_level");
  int _default_level = kDefaultLevel;
  if (de_level)
  {
    _default_level = atoi(de_level->FirstChild()->Value());
  }


  TiXmlElement *time_interval = root_element->FirstChildElement("time_interval");
  if (!time_interval) {
    WriteLog(kLogFatal, "time_interval not set");
    return -1;
  }
  time_interval_ = atoi(time_interval->FirstChild()->Value());
  if (time_interval_ <= 0) {
    WriteLog(kLogFatal, "time_interval invalid:%d", time_interval_);
    return -1;
  }

  /// 解析anti spider
  TiXmlElement *anti_spider_elem = root_element->FirstChildElement("anti_spider");
  if (anti_spider_elem) {
    string ip;
    int port = 0;
    int time_out = 0;

    TiXmlElement *p_elem = anti_spider_elem->FirstChildElement("ip");
    if (p_elem) {
      ip = p_elem->FirstChild()->Value();
    } else {
      WriteLog(kLogFatal, "no anti_spider.ip in xml");
      return -1;
    }

    p_elem = anti_spider_elem->FirstChildElement("port");
    if (p_elem) {
      port = atoi(p_elem->FirstChild()->Value());
    } else {
      WriteLog(kLogFatal, "no anti_spider.port in xml");
      return -1;
    }

    p_elem = anti_spider_elem->FirstChildElement("time_out");
    if (p_elem) {
      time_out = atoi(p_elem->FirstChild()->Value());
    } else {
      WriteLog(kLogFatal, "no anti_spider.time_out in xml");
      return -1;
    }

    if (port <= 0 || time_out <= 0) {
      WriteLog(kLogFatal, "anti_spider int param invalid:%d %d",
               port, time_out);
      return -1;
    }

    anti_spider_config_.ip_ = ip;
    anti_spider_config_.port_ = port;
    anti_spider_config_.time_out_ = time_out;
  } else {
    WriteLog(kLogFatal, "no anti_spider in xml");
    return -1;
  }

  /// 解析样式styles
  TiXmlElement *styles = root_element->FirstChildElement("styles");
  if (!styles) {
    WriteLog(kLogFatal, "styles not set");
    return -1;
  }
  TiXmlElement *p_elem = styles->FirstChildElement("style");
  while (p_elem) {
    int width = 0;
    int height = 0;
    int ptsize = 0;
    string back_color, font_color;
    RGBColor back_color_rgb, font_color_rgb;
    string font_file;
    int gap = 0;
    int long_num = 0;
    int line_back_num = 0;
    int line_font_num = 0;

    TiXmlAttribute *id_attribute = p_elem->FirstAttribute();
    string sstyle = id_attribute->Value();

    TiXmlElement *value = p_elem->FirstChildElement("width");
    if (!value) {
      WriteLog(kLogFatal, "width not set");
      return -1;
    }
    width = atoi(value->FirstChild()->Value());

    value = p_elem->FirstChildElement("height");
    if (!value) {
      WriteLog(kLogFatal, "height not set");
      return -1;
    }
    height = atoi(value->FirstChild()->Value());

    value = p_elem->FirstChildElement("ptsize");
    if (!value) {
      WriteLog(kLogFatal, "ptsize not set");
      return -1;
    }
    ptsize = atoi(value->FirstChild()->Value());

    value = p_elem->FirstChildElement("back_color");
    if (!value) {
      WriteLog(kLogFatal, "back color not set");
      return -1;
    }
    back_color = value->FirstChild()->Value();
    if (ParseColor(back_color, &back_color_rgb) < 0) {
      WriteLog(kLogFatal, "ParseColor:%s failed", back_color.c_str());
      return -1;
    }

    value = p_elem->FirstChildElement("font_color");
    if (!value) {
      WriteLog(kLogFatal, "font color not set");
      return -1;
    }
    font_color = value->FirstChild()->Value();
    if (ParseColor(font_color, &font_color_rgb) < 0) {
      WriteLog(kLogFatal, "ParseColor:%s failed", font_color.c_str());
      return -1;
    }

    value = p_elem->FirstChildElement("font_file");
    if (!value) {
      WriteLog(kLogFatal, "font file not set");
      return -1;
    }
    font_file = value->FirstChild()->Value();

    value = p_elem->FirstChildElement("gap");
    if (!value) {
      WriteLog(kLogFatal, "gap not set");
      return -1;
    }
    gap = atoi(value->FirstChild()->Value());

    value = p_elem->FirstChildElement("long_num");
    if (!value) {
      WriteLog(kLogFatal, "long num not set");
      return -1;
    }
    long_num = atoi(value->FirstChild()->Value());

    value = p_elem->FirstChildElement("line_back_num");
    if (value) {
      line_back_num = atoi(value->FirstChild()->Value());
      if (line_back_num > 0) {
        WriteLog(kLogFatal, "invalid line_back_num:%d", line_back_num);
        return -1;
      }
    }
    value = p_elem->FirstChildElement("line_font_num");
    if (value) {
      line_font_num = atoi(value->FirstChild()->Value());
      if (line_font_num > 0) {
        WriteLog(kLogFatal, "invalid line_back_num:%d", line_font_num);
        return -1;
      }
    }

    Style2ConfigMap::iterator it = style_map_.find(sstyle);
    if (it == style_map_.end()) {
      StyleConfig *p_style = new StyleConfig();
      p_style->SetBackColor(back_color_rgb);
      p_style->SetFontColor(font_color_rgb);
      p_style->SetWidth(width);
      p_style->SetHeight(height);
      p_style->SetPtsize(ptsize);
      p_style->SetFontFile(font_file);
      p_style->SetGap(gap);
      if (long_num == 1)
        p_style->SetLongNum(true);
      else
        p_style->SetLongNum(false);
      p_style->SetLineBackNum(line_back_num);
      p_style->SetLineFontNum(line_font_num);
      style_map_.insert(Style2ConfigMap::value_type(sstyle, p_style));
    } else {
      StyleConfig *&p_style = it->second;
      p_style->SetBackColor(back_color_rgb);
      p_style->SetFontColor(font_color_rgb);
      p_style->SetWidth(width);
      p_style->SetHeight(height);
      p_style->SetPtsize(ptsize);
      p_style->SetFontFile(font_file);
      p_style->SetGap(gap);
      if (long_num == 1)
        p_style->SetLongNum(true);
      else
        p_style->SetLongNum(false);
      p_style->SetLineBackNum(line_back_num);
      p_style->SetLineFontNum(line_font_num);
    }

    p_elem = p_elem->NextSiblingElement("style");
  }
  if (style_map_.end() == style_map_.find(_default_style)) {
    WriteLog(kLogFatal, "no default style");
    return -1;
  }
  default_style_ = style_map_[_default_style];

  /// 解析封禁级别levels
  TiXmlElement* levels = root_element->FirstChildElement("levels");
  if (!levels) {
    WriteLog(kLogFatal, "levels not set");
    return -1;
  }
  p_elem = levels->FirstChildElement("level");
  while (p_elem) {
    TiXmlAttribute *id_attribute = p_elem->FirstAttribute();
    int level = atoi(id_attribute->Value());
    int line_back_num = 0;
    int line_font_num = 0;
    int split_num = 0;
    int indent = 0;
    string back_color;
    string font_color;
    RGBColor back_color_rgb;
    RGBColor font_color_rgb = RGBColor(0, 0, 0);
    int gap = 0;
    int cluster_ch = 0;

    TiXmlElement *value;
    value = p_elem->FirstChildElement("line_back_num");
    if (!value) {
      WriteLog(kLogFatal, "no line_back_num in level:%d", level);
      return -1;
    }
    line_back_num = atoi(value->FirstChild()->Value());
    value = p_elem->FirstChildElement("line_font_num");
    if (!value) {
      WriteLog(kLogFatal, "no line_font_num in level:%d", level);
      return -1;
    }
    line_font_num = atoi(value->FirstChild()->Value());
    value = p_elem->FirstChildElement("split_num");
    if (!value) {
      WriteLog(kLogFatal, "no split_num in level:%d", level);
      return -1;
    }
    split_num = atoi(value->FirstChild()->Value());
    value = p_elem->FirstChildElement("indent");
    if (!value) {
      WriteLog(kLogFatal, "no indent in level:%d", level);
      return -1;
    }
    indent = atoi(value->FirstChild()->Value());
    value = p_elem->FirstChildElement("back_color");
    if (value) {
      back_color = value->FirstChild()->Value();
      if (ParseColor(back_color, &back_color_rgb) < 0)
        return -1;
    }
    value = p_elem->FirstChildElement("font_color");
    if (value) {
      font_color = value->FirstChild()->Value();
      if (ParseColor(font_color, &font_color_rgb) < 0)
        return -1;
    }
    value = p_elem->FirstChildElement("gap");
    if (!value) {
      WriteLog(kLogFatal, "no gap in level:%d", level);
      return -1;
    }
    gap = atoi(value->FirstChild()->Value());

    value = p_elem->FirstChildElement("cluster_ch");
    if (!value) {
      WriteLog(kLogFatal, "no cluster_ch in level:%d", level);
      return -1;
    }
    cluster_ch = atoi(value->FirstChild()->Value());

    Level2ConfigMap::iterator it = level_map_.find(level);
    if (it == level_map_.end()) {
      LevelConfig *plevel = new LevelConfig();
      plevel->SetLineBackNum(line_back_num);
      plevel->SetLineFontNum(line_font_num);
      plevel->SetSplitNum(split_num);
      plevel->SetIndent(indent);
      plevel->SetBackColor(back_color_rgb);
      plevel->SetFontColor(font_color_rgb);
      plevel->SetGap(gap);
      plevel->SetClusterCh(cluster_ch);
      level_map_.insert(Level2ConfigMap ::value_type(level, plevel));
    } else {
      LevelConfig *&plevel = it->second;
      plevel->SetLineBackNum(line_back_num);
      plevel->SetLineFontNum(line_font_num);
      plevel->SetSplitNum(split_num);
      plevel->SetIndent(indent);
      plevel->SetBackColor(back_color_rgb);
      plevel->SetFontColor(font_color_rgb);
      plevel->SetGap(gap);
      plevel->SetClusterCh(cluster_ch);
    }

    p_elem = p_elem->NextSiblingElement("level");
  }
  for (int i = 0; i < kTotalLevel; i++) {
    if (level_map_.end() == level_map_.find(i)) {
      WriteLog(kLogFatal, "no level:%d in levelconfig", i);
      return -1;
    }
  }

  if (level_map_.end() == level_map_.find(_default_level)) {
    WriteLog(kLogFatal, "no default level");
    return -1;
  }
  default_level_ = level_map_[_default_level];

  /// 解析字体文件列表
  TiXmlElement *fonts = root_element->FirstChildElement("fonts");
  if (fonts) {
    TiXmlElement *p_elem = fonts->FirstChildElement("path");
    while (p_elem) {
      font_list_.push_back(p_elem->FirstChild()->Value());
      p_elem = p_elem->NextSiblingElement("path");
    }
  }
  if (font_list_.empty()) {
    WriteLog(kLogFatal, "fonts empty");
    return -1;
  }

  /// 解析加解密配置
  TiXmlElement *des_decode = root_element->FirstChildElement("des_decode");
  if (des_decode) {
    string key;
    int time_limit = 0;
    int rotate_step = 0;
    int check_sum_base = 0;
    int check_sum_len = 0;

    TiXmlElement *p_elem = des_decode->FirstChildElement("key");
    if (p_elem) {
      key = p_elem->FirstChild()->Value();
    } else {
      WriteLog(kLogFatal, "no des_decode.key in xml");
      return -1;
    }

    p_elem = des_decode->FirstChildElement("time_limit");
    if (p_elem) {
      time_limit = atoi(p_elem->FirstChild()->Value());
    } else {
      WriteLog(kLogFatal, "no des_decode.time_limit in xml");
      return -1;
    }

    p_elem = des_decode->FirstChildElement("rotate_step");
    if (p_elem) {
      rotate_step = atoi(p_elem->FirstChild()->Value());
    } else {
      WriteLog(kLogFatal, "no des_decode.rotate_step in xml");
      return -1;
    }

    p_elem = des_decode->FirstChildElement("check_sum_base");
    if (p_elem) {
      check_sum_base = atoi(p_elem->FirstChild()->Value());
    } else {
      WriteLog(kLogFatal, "no des_decode.check_sum_base in xml");
      return -1;
    }

    p_elem = des_decode->FirstChildElement("check_sum_len");
    if (p_elem) {
      check_sum_len = atoi(p_elem->FirstChild()->Value());
    } else {
      WriteLog(kLogFatal, "no des_decode.check_sum_len in xml");
      return -1;
    }

    if (key.size() < 4) {
      WriteLog(kLogFatal, "key:%s invalid", key.c_str());
      return -1;
    }
    if (time_limit <= 0 || rotate_step < 0 || check_sum_base <= 0 || check_sum_len <= 0) {
      WriteLog(kLogFatal, "int param invalid:%d %d %d %d",
               time_limit, rotate_step, check_sum_base, check_sum_len);
      return -1;
    }

    if (check_sum_base != pow(10, check_sum_len)) {
      WriteLog(kLogFatal, "check_sum_base:%d != pow(10, check_sum_len:%d)",
               check_sum_base, check_sum_len);
      return -1;
    }

    des_config_.key_ = key;
    des_config_.time_limit_ = time_limit;
    des_config_.rotate_step_ = rotate_step;
    des_config_.check_sum_base_ = check_sum_base;
    des_config_.check_sum_len_ = check_sum_len;
  } else {
    WriteLog(kLogFatal, "no des_decode in xml");
    return -1;
  }

  pid_t pid = getpid();
  WriteLog(kLogNotice, "process:%d ImgConfig LoadConf OK", pid);
  return 0;
}

const string &ImgConfig::GetRandFont() {
  int font_num = font_list_.size();
  assert(font_num > 0);
  int pos = random() % font_num;
  return font_list_[pos];
}

LevelConfig *ImgConfig::GetLevelConf(int32_t level) {
  if (level < 0 || level >= 10) {
    //WriteLog(kLogFatal, "level:%d error in GetLevelConf", level);
    return default_level_;
  }
  return level_map_[level];
}

StyleConfig *ImgConfig::GetStyleConf(const string & c) {
  Style2ConfigMap::iterator it = style_map_.find(c);
  if (it != style_map_.end())
    return it->second;
  return default_style_;
}

int ImgConfig::ParseColor(const string &rgb, RGBColor *p_color) {
  vector<string> item_list;
  Text::Segment(rgb, ",", &item_list);
  if (item_list.size() != 3) {
    WriteLog(kLogFatal, "rgb color:%s invalid", rgb.c_str());
    return -1;
  }
  int red = Text::StrToInt(item_list[0]);
  int green = Text::StrToInt(item_list[1]);
  int blue = Text::StrToInt(item_list[2]);

  *p_color = RGBColor(red, green, blue);

  return 0;
}

int ImgConfigMgr::Init(const string &conf_path) {
  conf_path_ = conf_path;
  for (int i = 0; i < 2; i++) {
    if (img_config_[i].Init(conf_path_) < 0)
      return -1;
  }

  return 0;
}

int ImgConfigMgr::UpdateConf() {
  lock_.Lock();
  int next_index = (cur_index_+1)%2;
  int ret = img_config_[next_index].LoadConf(conf_path_);

  if (ret < 0) {
    WriteLog(kLogFatal, "UpdateConf:%d failed", next_index);
  } else {
    WriteLog(kLogNotice, "UpdateConf:%d OK", next_index);
    cur_index_ = next_index;
  }
  lock_.Unlock();

  return ret;
}
}}}
