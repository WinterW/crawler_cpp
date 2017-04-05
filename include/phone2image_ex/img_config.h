/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/img_config.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_CONFIG_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_CONFIG_H_

#include <string.h>
#include <backward/hash_fun.h>

#include <unordered_map>
#include <map>
#include <string>
#include <vector>

#include "util/thread/mutex.h"
#include "phone2image_ex/struct_def.h"

namespace ganji { namespace crawler { namespace phone2image {
struct StringHash {
  size_t operator()(const std::string &key) const {
    return __gnu_cxx::__stl_hash_string(key.c_str());
  }
  bool operator()(const std::string &s1, const std::string &s2) const {
    return strcmp(s1.c_str(), s2.c_str()) == 0;
  }
};

typedef std::unordered_map<int, LevelConfig *> Level2ConfigMap;
typedef std::unordered_map<std::string, StyleConfig *, StringHash, StringHash> Style2ConfigMap;

/**
 * @class ImgConfig
 * @brief 记录配置文件信息
 */
class ImgConfig {
 public:
  ImgConfig()
    : default_style_(NULL),
    default_level_(NULL),
    thread_num_(0),
    rand_font_level_(10),
    check_domain_(false),
    exclude_inner_ip_(true),
    time_interval_(0) {
    level_map_.rehash(30);
    style_map_.rehash(100);
  }

  ~ImgConfig() {
  }

  int Init(const std::string &conf_path);

  int GetThreadNum() { return thread_num_; }

  int GetRandFontLevel() { return rand_font_level_; }

  const std::string &GetUrlPrefix() const { return url_prefix_; }

  bool GetCheckDomain() const { return check_domain_; }

  bool GetExcludeInnerIp() const { return exclude_inner_ip_; }

  int GetTimeInterval() const { return time_interval_; }

  const std::string &GetRandFont();

  LevelConfig *GetLevelConf(int32_t level);

  StyleConfig *GetStyleConf(const std::string &c);

  DesConfig *GetDesConfig() { return &des_config_; }

  AntiSpiderConfig *GetAntiSpiderConfig() { return &anti_spider_config_; }

  /// @brief 加载配置文件
  int LoadConf(const std::string &conf_path);

 private:
  /// @brief 解析RGB格式的color，转换为color值
  /// @param[in] rgb 逗号分隔开的rgb值
  /// @param[out] p_color rgb_color
  int ParseColor(const std::string &rgb, RGBColor *p_color);

 private:
  /// 字体文件列表
  std::vector<std::string> font_list_;
  /// level => config
  Level2ConfigMap level_map_;
  /// style => config
  Style2ConfigMap style_map_;
  /// 默认的style
  StyleConfig *default_style_;
  /// 默认的Level
  LevelConfig *default_level_;
  /// des相关配置
  DesConfig des_config_;
  AntiSpiderConfig anti_spider_config_;
  int thread_num_;
  /// 使用随机字体的起始level
  int rand_font_level_;
  /// 号码图片url的前缀
  std::string url_prefix_;
  /// 是否校验域名
  bool check_domain_;
  /// 是否屏蔽内网ip
  bool exclude_inner_ip_;
  /// 输出统计信息的时间间隔，单位：秒
  int time_interval_;
};

/**
 * @class ImgConfigMgr
 * @brief 管理ImgConfig
 */
class ImgConfigMgr {
 public:
  ImgConfigMgr()
    : cur_index_(0) {
  }

  int Init(const std::string &conf_path);

  int UpdateConf();

  int GetThreadNum() { return img_config_[cur_index_].GetThreadNum(); }

  int GetRandFontLevel() { return img_config_[cur_index_].GetRandFontLevel(); }

  const std::string &GetUrlPrefix() const { return img_config_[cur_index_].GetUrlPrefix(); }

  bool GetCheckDomain() const { return img_config_[cur_index_].GetCheckDomain(); }

  bool GetExcludeInnerIp() const { return img_config_[cur_index_].GetExcludeInnerIp(); }

  int GetTimeInterval() const { return img_config_[cur_index_].GetTimeInterval(); }

  const std::string &GetRandFont() { return img_config_[cur_index_].GetRandFont(); }

  LevelConfig *GetLevelConf(int32_t level) { return img_config_[cur_index_].GetLevelConf(level); }

  StyleConfig *GetStyleConf(const std::string &c) { return img_config_[cur_index_].GetStyleConf(c); }

  DesConfig *GetDesConfig() { return img_config_[cur_index_].GetDesConfig(); }

  AntiSpiderConfig *GetAntiSpiderConfig() { return img_config_[cur_index_].GetAntiSpiderConfig(); }

 private:
  std::string conf_path_;
  /// 双buffer
  ImgConfig img_config_[2];
  ganji::util::thread::Mutex lock_;
  int cur_index_;
};
}}}

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_CONFIG_H_

