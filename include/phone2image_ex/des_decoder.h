/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/des_decoder.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_DES_DECODER_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_DES_DECODER_H_

#include <string>
#include <map>

namespace ganji { namespace crawler { namespace phone2image {
class ImgConfigMgr;
class EncDec;

/**
 * @class DesDecoder
 * @brief 使用des解密算法对号码图片url进行解密
 */
class DesDecoder {
 public:
  DesDecoder();

  ~DesDecoder();

  int Init(ImgConfigMgr *p_img_config_mgr);

  /// @brief 使用des解密算法对号码图片url进行解密
  /// @param[in] image_id 号码图片url中的id
  /// @param[in] args_map 号码图片url对应的参数
  /// @return 0:success -1:failure
  /// 解析后的参数可使用下述Getxxx函数获得
  int Decode(const std::string &image_id, const std::map<std::string, std::string> &args_map);

  const std::string &GetPhone();

  int GetLevel();

  const std::string &GetStyle();

  const std::string &GetDomain();

  const std::string &GetCatMajor();

  char * GetErrBuf();

 private:
  EncDec *p_enc_dec_;
};
}}}

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_DES_DECODER_H_

