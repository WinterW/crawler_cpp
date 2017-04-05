/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/decoder_test.cc
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
#include <stdlib.h>
#include <string>
#include <map>


#include "ganji/crawler/phone2image_ex/decode_interface.h"

namespace ganji { namespace crawler { namespace phone2image {
class Decoder_test : public Decoder {
 public:

  Decoder_test() {
    style_ = "0";
  }

  ~Decoder_test() {
  }

  int Init() {
    return 0;
  }

  bool Decode(const std::string &key, const std::map<std::string, std::string> &args_map) {
    phone_ = "12345678901";
    width_ = 127;
    height_ = 20;
    level_ = 1;
    std::map<std::string, std::string>::const_iterator it_ = args_map.find("phone");
    if (it_ != args_map.end())
      phone_ = it_->second;
    it_ = args_map.find("width");
    if (it_ != args_map.end())
      width_ = atol(it_->second.c_str());
    it_ = args_map.find("height");
    if (it_ != args_map.end())
      height_ = atol(it_->second.c_str());
    it_ = args_map.find("level");
    if (it_ != args_map.end())
      level_ = atol(it_->second.c_str());

    if (width_ < 10 || width_ > 1024)
      width_ = 130;
    if (height_ < 10 || height_ > 800)
      height_ = 25;

    it_ = args_map.find("c");
    if (it_ != args_map.end())
      style_ = it_->second;

    return true;
  }

  void GetPhone(std::string * _phone) {
    *_phone = phone_;
  }

  int GetLevel() {
    return level_;
  }

  void GetStyle(std::string *p_style) {
    *p_style = style_;
  }

 protected:
  std::string phone_;
  int32_t width_;
  int32_t height_;
  int32_t level_;
  std::string style_;
};

Decoder * Decoder::GetDecoder() {
  return new Decoder_test();
}
}}}
