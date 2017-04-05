/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/phone_level.cc
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @author  lisizhong
 * @author  luomeiqing
 * @date    2012-11-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "phone2image_ex/phone_img.h"

#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <gdfontg.h>
#include <math.h>

#include <string>
#include <list>
#include <algorithm>

#include "util/log/thread_fast_log.h"

#include "phone2image_ex/img_config.h"

namespace ganji { namespace crawler { namespace phone2image {
using std::list;
using std::string;
using std::vector;
namespace FastLog = ganji::util::log::ThreadFastLog;
using FastLog::kLogFatal;
using FastLog::WriteLog;

int PhoneImg::Init(ImgConfigMgr *p_img_config_mgr) {
  p_img_config_mgr_ = p_img_config_mgr;
  gdFontCacheSetup();

  return 0;
}

int PhoneImg::GetFontFile(int level, const StyleConfig *style_config, string *p_font_file) {
  *p_font_file = style_config->GetFontFile();
  // int rand_font_level = p_img_config_mgr_->GetRandFontLevel();
  // if (level >= rand_font_level)
  //     *p_font_file = p_img_config_mgr_->GetRandFont();

  if (p_font_file->empty())
    return -1;
  else
    return 0;
}

void PhoneImg::GetPhoneSplit(int level,
                             const StyleConfig *style_config,
                             const string &phone,
                             int32_t col,
                             vector<PhoneItem> *p_phone_list) {
  const string &font_file = style_config->GetFontFile();
  int rand_font_level = p_img_config_mgr_->GetRandFontLevel();
  bool is_rand_font = false;
  if (level >= rand_font_level)
    is_rand_font = true;

  if (col < 1)
    col = 1;
  int32_t space = phone.size() / col;
  if (space < 1)
    space = 1;
  int32_t index_ = 0;
  PhoneItem item;
  while (true) {
    if (index_ >= static_cast<int32_t>(phone.size()))
      break;

    if (is_rand_font)
      item.font_ = p_img_config_mgr_->GetRandFont();
    else
      item.font_ = font_file;
    int32_t leave_len_ = phone.size() - index_;
    if (leave_len_ < space)
      space = leave_len_;
    item.str_ = string(phone.c_str() + index_, space);
    index_ += space;
    (*p_phone_list).push_back(item);
  }
}

int PhoneImg::DrawText(int level,
                       const LevelConfig *level_config,
                       const StyleConfig *style_config,
                       const string &phone,
                       int32_t height,
                       int font_color,
                       gdImagePtr &im) {
  vector<PhoneItem> phones;
  GetPhoneSplit(level, style_config, phone, level_config->GetSplitNum(), &phones);
  int x_pos = 0;
  int brect[8];
  unsigned int seed = 0;

  for (int i = 0; i < static_cast<int>(phones.size()); i++) {
    double angle = 0.0;
    if (phones.size() > 300) {
      int r1 = random();
      seed = r1;
      angle = r1 % 50;
      angle *= 0.004;
      if (r1%2 == 0)
        angle *= -1;
    }

    /// gdImageStringFT(gdImagePtr im, int *brect, int fg, char *fontname, double ptsize, double angle, int x, int y, char *string)
    char *err = gdImageStringFT(im,
                                &brect[0],
                                font_color,
                                const_cast<char*>(phones[i].font_.c_str()),
                                height*0.8,
                                angle,
                                x_pos,
                                height-height*0.1,
                                const_cast<char*>(phones[i].str_.c_str()));
    if (err) {
      WriteLog(kLogFatal, "DrawText error:%s", err);
      return -1;
    }

    x_pos += brect[4]-brect[0];
    x_pos -= level_config->GetIndent();
  }

  return 0;
}

int PhoneImg::DrawTextGap(int level,
                          const LevelConfig *level_config,
                          const StyleConfig *style_config,
                          const string &phone,
                          int32_t height,
                          int font_color,
                          gdImagePtr *im,
                          gdImagePtr *im_new) {
  int ptsize = style_config->GetPtsize();
  const int start_pos = 3;
  int x_pos = start_pos;
  int minus_flag = 0;
  int phone_len = phone.size();
  int gap_level = level_config->GetGap();
  int gap_style = style_config->GetGap();
  int gap = gap_level + gap_style;
  int cluster_ch = level_config->GetClusterCh();
  int brect[8];
  double angle = 0;
  string font_file;
  GetFontFile(level, style_config, &font_file);
  if(cluster_ch >= 5)
  {
    int dm = cluster_ch  - 2;
    cluster_ch = random()%dm + 3;
  }

  int cur_cluster_ch = 1;
  if (phone_len > 13 && style_config->GetLongNum()) {
    ptsize = std::max(9, ptsize - (phone_len - 12));
    // gap = static_cast<int>(ceilf(static_cast<float>(ptsize) * 4 / 9));
    gap = static_cast<int>(static_cast<float>(ptsize) * 4 / 9);
  }

  if (string::npos == phone.find('-') && phone_len == 11) {
     minus_flag = random() % 11;
   }

  for (int i = 0; i < phone_len; i++) {
    string letter = phone.substr(i, 1);
    char ch = phone[i];
    int shift = 0;
    int cur_size = 0;
    if (ch == '1') {
      cur_size = ptsize + 2;
      shift = 1;
    } else {
      shift = random() % 3 - 1;
      cur_size = ptsize + shift;
    }

    /// 在数字中插入-
    if ((minus_flag > 3 && i == 3) ||
        (minus_flag > 7 && i == 7)) {
      /// gdImageStringFT(gdImagePtr im, int *brect, int fg, char *fontname, double ptsize, double angle, int x, int y, char *string)
      x_pos += 2;
      char *err = gdImageStringFT(*im,
                                  &brect[0],
                                  font_color,
                                  const_cast<char*>(font_file.c_str()),
                                  ptsize + 2,
                                  0.0,
                                  x_pos,
                                  height*0.9,
                                  const_cast<char *>("-"));
      if (err) {
        WriteLog(kLogFatal, "DrawText error:%s", err);
        return -1;
      }
      x_pos += brect[2] - brect[0] - gap;
      cur_cluster_ch = 1;
    }

    if(x_pos > 3 && (ch == '1'))
    {
      x_pos -= 1;
    }
    
    //angle = -1 *(0.015 * (random()%11));

    char *err = gdImageStringFT(*im,
                                &brect[0],
                                font_color,
                                const_cast<char*>(font_file.c_str()),
                                ptsize,
                                angle,
                                x_pos,
                                height*0.9+shift,
                                const_cast<char *>(letter.c_str()));
    if (err) {
      WriteLog(kLogFatal, "DrawText error:%s", err);
      return -1;
    }

    if (ch != '-') {
      if (cur_cluster_ch < cluster_ch) {
        x_pos += brect[2] - brect[0] - gap + random()%2;
        cur_cluster_ch++;
      } else {
        x_pos += brect[2] - brect[0];
        int denom = static_cast<int>(static_cast<float>(gap)*3/4)+1;
        x_pos = x_pos - random() % denom + 1;
        cur_cluster_ch = 1;
      }
    } else {
      x_pos += brect[2] - brect[0] - gap;
    }
  }

  /// 把im中的字符部分copy到新的图片中
  /// XXX 新的图片长度设定为x_pos+trailing_width，保证末尾的字符完全copy
  const int trailing_width = 3;
  height = gdImageSY(*im);
  *im_new = gdImageCreate(x_pos + start_pos + trailing_width, height);
  gdImageCopy(*im_new, *im, 0, 0, 0, 0, x_pos + start_pos + trailing_width, height);

  return 0;
}

/// Y\X  0   1   2   3
///   -----------------
/// 0 | 0 | 1 | 2 | 3 |
///   -----------------
/// 1 | 4 | 5 | 6 | 7 |
///   -----------------
/// 4种组合: 区间0<=>6, 区间1<=>7，区间4<=>2，区间6<=>3
void PhoneImg::DrawFirstLine(int32_t width, int32_t height, int color, gdImagePtr *im) {
  int delim_x0 = random()%(width/4);
  int delim_x1 = random()%(width/4) + width/4;
  int delim_x2 = random()%(width/4) + width/2;
  int delim_x3 = random()%(width/4) + width*3/4;
  int delim_y0 = random()%(height/2);
  int delim_y1 = random()%(height/2) + height/2;

  int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
  int r = random() % 4;
  if (r == 0) {
    x0 = delim_x0;
    x1 = delim_x2;
    y0 = delim_y0;
    y1 = delim_y1;
  } else if (r == 1) {
    x0 = delim_x1;
    x1 = delim_x3;
    y0 = delim_y0;
    y1 = delim_y1;
  } else if (r == 2) {
    x0 = delim_x0;
    x1 = delim_x2;
    y0 = delim_y1;
    y1 = delim_y0;
  } else {
    x0 = delim_x1;
    x1 = delim_x3;
    y0 = delim_y1;
    y1 = delim_y0;
  }

  gdImageLine(*im, x0, y0, x1, y1, color);
}

void PhoneImg::DrawLines(int32_t num, const int32_t width, const int32_t height, int color, gdImagePtr *im) {
  if (num <= 0)
    return;

  /// XXX 因为下面随机画线可能不会画在字符上，所以第一条线需保证画在字符数
  DrawFirstLine(width, height, color, im);

  for (int i = 1; i < num; i++) {
    int r0 = random();
    int r1 = random();
    gdImageLine(*im, r0%width, r0%height, r1%width, r1%height, color);
  }
}

/// 获取 对应类别对应级别的 电话号码配置
void PhoneImg::GetImageConfig(const LevelConfig * level,
                              const StyleConfig * style,
                              int *p_width,
                              int *p_height,
                              RGBColor *p_back_color,
                              RGBColor *p_font_color) {
  *p_width = style->GetWidth();
  *p_height = style->GetHeight();
  // *p_back_color = level->GetBackColor();
  // if (*p_back_color != style->GetBackClolor()) {
    *p_back_color = style->GetBackClolor();
  // }
  // *p_font_color = level->GetFontColor();
  // if (*p_font_color != style->GetFontColor()) {
    *p_font_color = style->GetFontColor();
  // }
}

int PhoneImg::PhoneToImage(int level,
                           const string &style,
                           const string &phone,
                           string *image_str) {
  const LevelConfig *level_config = p_img_config_mgr_->GetLevelConf(level);
  const StyleConfig *style_config = p_img_config_mgr_->GetStyleConf(style);
  int width = 0;
  int height = 0;

  /// 获取对应的图片配置
  GetImageConfig(level_config, style_config, &width, &height, &back_color_, &font_color_);

  gdImagePtr im;
  im = gdImageCreate(width, height);
  if (!im) {
    WriteLog(kLogFatal, "gdImageCreate() failed");
    return -1;
  }

  int back_color = gdImageColorAllocate(im, back_color_.red_, back_color_.green_, back_color_.blue_);
  int font_color = gdImageColorAllocate(im, font_color_.red_, font_color_.green_, font_color_.blue_);
  if (back_color < 0 || font_color < 0) {
    gdImageDestroy(im);
    WriteLog(kLogFatal, "gdImageColorAllocate() failed");
    return -1;
  }

  gdImagePtr im_new;
  if (DrawTextGap(level, level_config, style_config, phone, height, font_color, &im, &im_new) < 0) {
    /// XXX 不需要destroy im_new，因为gdImageCopy不会返回错误码
    gdImageDestroy(im);
    WriteLog(kLogFatal, "DrawTextGap() failed");
    return -1;
  }

  /// XXX 新图片的宽度
  int new_width = gdImageSX(im_new)+4;
  /// XXX style对应的图片较小，则线数量可以适量减少
  int back_num = level_config->GetLineBackNum() + style_config->GetLineBackNum();
  int font_num = level_config->GetLineFontNum() + style_config->GetLineFontNum();
  if (phone.size() > 13 && style_config->GetLongNum()) {
    back_num -= 1;
    font_num -= 1;
  }
  DrawLines(back_num, new_width, height, back_color, &im_new);
  DrawLines(font_num, new_width, height, font_color, &im_new);

  int image_size = 0;
  void *image_data = gdImagePngPtr(im_new, &image_size);
  *image_str = string(reinterpret_cast<const char*>(image_data), image_size);
  gdFree(image_data);
  gdImageDestroy(im_new);
  gdImageDestroy(im);

  /// DrawText(level, level_config, style_config, phone, height, font_color, im);
  /// DrawLines(level_config->GetLineBackNum(), width, height, im, back_color);
  /// DrawLines(level_config->GetLineFontNum(), width, height, im, font_color);
  /// int image_size =0;
  /// void *image_data = gdImagePngPtr(im,&image_size);
  /// *image_str = string(reinterpret_cast<const char*>(image_data), image_size);
  /// gdFree(image_data);
  /// gdImageDestroy(im);

  return 0;
}
}}}
