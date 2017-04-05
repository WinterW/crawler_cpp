/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/phone_img.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */
#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_PHONE_IMG_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_PHONE_IMG_H_

#include <gd.h>
#include <string>
#include <vector>

#include "phone2image_ex/struct_def.h"

namespace ganji { namespace crawler { namespace phone2image {
class ImgConfigMgr;

/**
 * @class PhoneImg
 * @brief 实际的生成图片类
 */
class PhoneImg {
 public:
  PhoneImg()
    : p_img_config_mgr_(NULL) {
  }

  int Init(ImgConfigMgr *p_img_config_mgr);

  /// @brief 生成电话号码图片
  /// @param[in] level 封禁级别
  /// @param[in] style 样式
  /// @param[in] phone 联系方式
  /// @param[out] image_str 图片的二进制数据
  int PhoneToImage(int level,
                   const std::string &style,
                   const std::string &phone,
                   std::string *image_str);

 private:
  /// @brief 获取level/style对应的字体文件
  int GetFontFile(int level, const StyleConfig *style_config, std::string *p_font_file);

  void GetPhoneSplit(int level,
                     const StyleConfig *style_config,
                     const std::string &phone,
                     int32_t col,
                     std::vector<PhoneItem> *p_phone_list);

  int DrawText(int level,
               const LevelConfig *level_config,
               const StyleConfig *style_config,
               const std::string &phone,
               int32_t height,
               int font_color,
               gdImagePtr &im);

  /// @brief 随机调整字符间距，画出字符串
  /// @param[in] level 封禁级别
  /// @param[in] level_config 级别配置器
  /// @param[in] style_config 样式配置器
  /// @param[in] phone 联系方式
  /// @param[in] height 待生成图片的高度
  /// @param[in] font_color 字体颜色
  /// @param[out] im 画出字符串的图片
  /// @param[out] im_new 对im切除后面的留白的图片
  int DrawTextGap(int level,
                  const LevelConfig *level_config,
                  const StyleConfig *style_config,
                  const std::string &phone,
                  int32_t height,
                  int font_color,
                  gdImagePtr *im,
                  gdImagePtr *im_new);

  /// @brief 画第一条线
  void DrawFirstLine(int32_t width, int32_t height, int color, gdImagePtr *im);

  /// @brief 随机画出num条线
  void DrawLines(int32_t num, const int32_t width, const int32_t height, int color, gdImagePtr *im);

  /// @brief 从类别/样式配置器中获取对应的配置
  /// @param[in] level 类别配置器
  /// @param[in] style 样式配置器
  /// @param[out] p_width 对应的width
  /// @param[out] p_height 对应的height
  /// @param[out] p_back_color 对应的底图颜色
  /// @param[out] p_font_color 对应的字体颜色
  void GetImageConfig(const LevelConfig *level,
                      const StyleConfig *style,
                      int *p_width,
                      int *p_height,
                      RGBColor *p_back_color,
                      RGBColor *p_font_color);

 private:
  ImgConfigMgr *p_img_config_mgr_;

  RGBColor back_color_;
  RGBColor font_color_;
};
}}}

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_PHONE_IMG_H_

