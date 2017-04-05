/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/struct_def.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  lisizhong
 * @date    2011-10-31
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */
#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_STRUCT_DEF_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_STRUCT_DEF_H_

#include <string>
#include <vector>

namespace ganji { namespace crawler { namespace phone2image {
/// 与PHP前端的接口参数
/// 参考:https://portal.corp.ganji.com/techteam/DocLib/%E4%BA%92%E8%81%94%E7%BD%91%E5%B9%B3%E5%8F%B0/%E4%BF%A1%E6%81%AF%E6%8A%BD%E5%8F%96%E7%BB%84/%E9%98%B2%E6%8A%93%E5%8F%96anti_spider/%E5%89%8D%E7%AB%AF%E4%B8%8E%E5%8F%B7%E7%A0%81%E5%9B%BE%E7%89%87%E7%94%9F%E6%88%90%E6%9C%8D%E5%8A%A1%E7%9A%84%E6%8E%A5%E5%8F%A3.aspx
const char kPhone[] = "phone";
const char kStyle[] = "style";
const char kCurTime[] = "cur_time";
const char kDomain[] = "domain";
const char kCatMajor[] = "cat_major";
/// 默认style
const char kDefaultStyle[] = "-1";
/// 级别总数，从0-9
const int kTotalLevel = 10;
const int kDefaultLevel = 5;
/// 配置文件
const char kConfFile[] = "conf.xml";

/**
 * @struct RGBColor
 * @brief 存放rgb color
 */
struct RGBColor {
  /// 默认为white
  RGBColor()
    : red_(255),
    green_(255),
    blue_(255) {
  }

  RGBColor(int red, int green, int blue)
    : red_(red),
    green_(green),
    blue_(blue) {
  }

  bool operator!=(const RGBColor &rgb_color) {
    if (red_ == rgb_color.red_ &&
        green_ == rgb_color.green_ &&
        blue_ == rgb_color.blue_)
      return false;
    return true;
  }

  int red_;
  int green_;
  int blue_;
};

/**
 * @struct DesConfig
 * @brief 配置文件中号码图片url生成算法相关配置
 */
struct DesConfig {
  DesConfig()
    : time_limit_(0),
    rotate_step_(0),
    check_sum_base_(0),
    check_sum_len_(0) {
  }

  std::string key_;           ///< 密钥
  int time_limit_;            ///< url时间戳与当前时间戳允许的最大时间差，单位:秒
  int rotate_step_;           ///< 循环左移的step
  int check_sum_base_;        /// 校验码
  int check_sum_len_;
};

/**
 * @struct DesConfig
 * @brief 配置文件中防抓取相关配置
 */
struct AntiSpiderConfig {
  AntiSpiderConfig()
    : port_(0),
    time_out_(10) {
  }

  std::string ip_;        ///< 防抓取后台服务的ip
  int port_;              ///< 防抓取后台服务的port
  int time_out_;          ///< 与防抓取后台的超时时间，单位:ms
};

struct PhoneItem {
  std::string str_;
  std::string font_;
};

/**
 * @struct StyleConfig
 * @brief 配置文件中style相关配置
 */
class StyleConfig {
 public:
  StyleConfig()
    : width_(0),
    height_(0),
    ptsize_(0),
    /// white
    back_color_(RGBColor()),
    /// bkack
    font_color_(RGBColor(0, 0, 0)),
    gap_(0),
    long_num_(false),
    line_back_num_(0),
    line_font_num_(0) {
  }

  void SetWidth(int n) { width_ = n; }
  int GetWidth() const { return width_; }

  void SetHeight(int n) { height_ = n; }
  int GetHeight() const { return height_; }

  void SetPtsize(int n) { ptsize_ = n; }
  int GetPtsize() const { return ptsize_; }

  void SetBackColor(const RGBColor &s) { back_color_ = s; }
  const RGBColor &GetBackClolor() const { return back_color_; }

  void SetFontColor(const RGBColor &s) { font_color_ = s; }
  const RGBColor &GetFontColor() const { return font_color_; }

  void SetFontFile(const std::string &font_file) { font_file_ = font_file; }
  const std::string &GetFontFile() const { return font_file_; }

  void SetGap(int gap) { gap_ = gap; }
  int GetGap() const { return gap_; }

  void SetLongNum(bool long_num) { long_num_ = long_num; }
  bool GetLongNum() const { return long_num_; }

  void SetLineBackNum(int32_t num) { line_back_num_ = num; }
  int32_t GetLineBackNum() const { return line_back_num_; }

  void SetLineFontNum(int32_t num) { line_font_num_ = num; }
  int32_t GetLineFontNum() const { return line_font_num_; }

 private:
  int width_;
  int height_;
  int ptsize_;
  RGBColor back_color_;
  RGBColor font_color_;
  std::string font_file_;
  int gap_;
  /// 是否对较长字符串进行缩放
  bool long_num_;
  int32_t line_back_num_;
  int32_t line_font_num_;
};

/**
 * @struct LevelConfig
 * @brief 配置文件中level相关配置
 */
class LevelConfig {
 public:
  LevelConfig()
    : line_back_num_(0),
    line_font_num_(0),
    split_num_(0),
    indent_(0),
    gap_(0),
    cluster_ch_(0) {
  }

  void SetLineBackNum(int32_t num) { line_back_num_ = num; }

  int32_t GetLineBackNum() const { return line_back_num_; }

  void SetLineFontNum(int32_t num) { line_font_num_ = num; }

  int32_t GetLineFontNum() const { return line_font_num_; }

  void SetSplitNum(int32_t num) { split_num_ = num; }

  int32_t GetSplitNum() const { return split_num_; }

  void SetIndent(int32_t num) { indent_ = num; }

  int32_t GetIndent() const { return indent_; }

  void SetBackColor(const RGBColor &color) { back_color_ = color; }

  const RGBColor &GetBackColor() const { return back_color_; }

  void SetFontColor(const RGBColor &color) { font_color_ = color; }

  const RGBColor &GetFontColor() const { return font_color_; }

  void SetGap(int gap) { gap_ = gap; }

  int GetGap() const { return gap_; }

  void SetClusterCh(int num) { cluster_ch_ = num; }

  int GetClusterCh() const { return cluster_ch_; }

 private:
  int32_t line_back_num_;
  int32_t line_font_num_;
  int32_t split_num_;
  int32_t indent_;
  /// 背景颜色
  RGBColor back_color_;
  /// 字体颜色
  RGBColor font_color_;
  /// 字符缩进的间距，值越大，越接近
  int gap_;
  /// 粘结在一起的字符数量
  int cluster_ch_;
};
}}}

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_STRUCT_DEF_H_
