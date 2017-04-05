/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/des_decoder.cc
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  lisizhogn
 * @date    2011-10-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "phone2image_ex/des_decoder.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <map>

#include "phone2image_ex/img_config.h"
#include "phone2image_ex/enc_dec.h"

namespace ganji { namespace crawler { namespace phone2image {
using std::string;
using ganji::crawler::phone2image::ImgConfig;
using ganji::crawler::phone2image::DesConfig;
using ganji::crawler::phone2image::EncDec;

DesDecoder::DesDecoder() {
  p_enc_dec_ = new EncDec();
}

DesDecoder::~DesDecoder() {
  if (p_enc_dec_)
    delete p_enc_dec_;
}

int DesDecoder::Init(ImgConfigMgr *p_img_config_mgr) {
  int ret = p_enc_dec_->Init(p_img_config_mgr);

  return ret;
}

int DesDecoder::Decode(const string &image_id, const map<string, string> &args_map) {
  int ret = p_enc_dec_->ParseParams(image_id, args_map);

  return ret;
}

const string &DesDecoder::GetPhone() {
  return p_enc_dec_->GetPhone();
}

int DesDecoder::GetLevel() {
  return p_enc_dec_->GetLevel();
}

const string &DesDecoder::GetStyle() {
  return p_enc_dec_->GetStyle();
}

const string &DesDecoder::GetDomain() {
  return p_enc_dec_->GetDomain();
}

const string &DesDecoder::GetCatMajor() {
  return p_enc_dec_->GetCatMajor();
}

char * DesDecoder::GetErrBuf() {
  return p_enc_dec_->GetErrBuf();
}
}}}
