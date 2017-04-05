/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/phone_level.cc
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */
#include <sys/time.h>
#include <unistd.h>
#include <Magick++.h>

#include <string>

#include "ganji/util/log/thread_fast_log.h"

#include "ganji/crawler/phone2image_ex/phone2image.h"
#include "ganji/crawler/phone2image_ex/img_builder.h"
#include "ganji/crawler/phone2image_ex/img_config.h"

namespace ganji { namespace crawler { namespace phone2image {
  
using namespace Magick;
using std::list;
using std::string;
using std::vector;
namespace FastLog = ganji::util::log::ThreadFastLog;
using FastLog::kLogFatal;
using FastLog::WriteLog;

struct phone_item {
  std::string str_;
  std::string font_;
};

void GetPhoneSplit(const string &phone, int32_t col, vector<phone_item> *p_phone_list) {
  if (col < 1)
    col = 1;
  int32_t space = phone.size() / col;
  if (space < 1)
    space = 1;
  int32_t index_ = 0;
  while (true) {
    if (index_ >= static_cast<int32_t>(phone.size()))
      break;

    phone_item item_;
    item_.font_ = ImgConfig::GetImgConfig()->GetRandFont();
    int32_t leave_len_ = phone.size() - index_;
    if (leave_len_ < space)
      space = leave_len_;
    item_.str_ = string(phone.c_str() + index_, space);
    index_ += space;
    (*p_phone_list).push_back(item_);
  }
}

void DrawText(const LevelConfig *level, const string &phone, int32_t height, Image *p_image) {

  TypeMetric metric;
  vector<phone_item> phones;
  GetPhoneSplit(phone, level->GetSplitNum(), &phones);
  Image &image_obj = *p_image;
  int x_pos = 0;

  for (int i = 0; i < static_cast<int>(phones.size()); i++) {
    //image_obj.font(phones[i].font_);
    list<Drawable> draw_list;
    // DrawableText drawText(x_pos, height - (height * 0.1), phones[i].str_);
    DrawableText draw_text(x_pos, height - (height * 0.1), phones[i].str_);
    // draw_list.push_back(img_config::getImgConfig()->getRandFont());
    draw_list.push_back(draw_text);
    image_obj.draw(draw_list);
    image_obj.fontTypeMetrics(phones[i].str_, &metric);
    x_pos += metric.textWidth();
    x_pos -= level->GetIndent();
  }
}

void DrawLines(int32_t num, const int32_t width, const int32_t height, Image *p_image) {
  if (num <= 0)
    return;

  Image &image_obj = *p_image;
  list<Drawable> draw_list;
  unsigned int seed = 0;
  for (int i = 0; i < num; i++) {
    int r1 = GetRand(seed);
    seed = r1;
    int r2 = GetRand(seed);
    // FastLog::WriteLog(kLogNone, "%d   %d", r1, r2);
    draw_list.push_back(DrawableLine(r1 % width, r1 % height, r2 % width, r2 % height));
    // image_.draw( DrawableLine(rand() % width, rand() % height, rand() % width, rand() % height));
    // image_.strokeColor("red"); // Outline color
    // image_.draw( DrawableCircle(100,100, 50,100) );
  }
  image_obj.draw(draw_list);
}

/// 获取 对应类别对应级别的 电话号码配置
void GetImageConfig(const LevelConfig * level,
                    const StyleConfig * style,
                    int *p_width,
                    int *p_height,
                    string *p_back_color,
                    string *p_font_color) {
  *p_width = style->GetWidth();
  *p_height = style->GetHeight();
  *p_back_color = level->GetBackColor();
  if (p_back_color->size() <= 1) {
    *p_back_color = style->GetBackClolor();
  }
  *p_font_color = level->GetFontColor();
  if (p_font_color->size() <= 1) {
    *p_font_color = style->GetFontColor();
  }
}
bool PhoneToImageInit()
{

  return true;
}
bool PhoneToImage(const LevelConfig *level,
                  const StyleConfig *style,
                  const string &phone,
                  string *image_str) {
  int width = 127;
  int height = 20;
  string back_color = "white";
  string font_color = "black";
  GetImageConfig(level, style, &width, &height, &back_color, &font_color);

  


  try {
    Image image_obj(Geometry(width, height), Color(back_color));
    image_obj.magick("png");
    image_obj.fontPointsize(height + height * 0.1);
    image_obj.strokeWidth(height / 15);

    image_obj.strokeColor(font_color);
    DrawText(level, phone, height, &image_obj);
    DrawLines(level->GetLineBackNum(), width, height, &image_obj);

    image_obj.strokeColor(back_color);
    DrawLines(level->GetLineBackNum(), width, height, &image_obj);

    Blob blob;
    image_obj.write(&blob);
    *image_str = string(reinterpret_cast<const char*>(blob.data()), blob.length());
    return true;
  } catch(const Exception &error_) {
    WriteLog(kLogFatal, "phone_to_image  faild: %s (width:%d,height:%d,back_color:%s,font_color:%s)",
             error_.what(), width, height, back_color.c_str(), font_color.c_str());
    return false;
  }

}

unsigned int GetRand(unsigned int seed) {
  struct timeval nowtimeval;
  gettimeofday(&nowtimeval, 0);

  unsigned int a = nowtimeval.tv_usec * nowtimeval.tv_sec + seed;
  return rand_r(&a);
}
}}}
