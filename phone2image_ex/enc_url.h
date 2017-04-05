/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/enc_url.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-04
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_ENC_URL_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_ENC_URL_H_

#include <string>
#include <map>
#include <set>
#include <vector>

#include "phone2image_ex/struct_def.h"
#include "phone2image_ex/enc_dec.h"

namespace ganji { namespace crawler { namespace phone2image {
/**
 * @class EncUrl
 * @brief 加密img，得到加密后的url
 */
class EncUrl {
 public:
  EncUrl() {
  }

  /// @brief 从文件中获取ip range
  int Init(ImgConfigMgr *p_img_config_mgr);

  /// @brief 解析http请求参数，加密相关信息，得到其id
  /// @param[in] p_env fcgi参数
  /// @param[in] query_str http请求参数
  /// @param[in] args_map http请求参数
  /// @param[out] p_img_id 加密后的号码图片id
  int GenImgUrl(char **p_env,
                const char *query_str,
                const std::map<std::string, std::string> &args_map,
                std::string *p_img_id);

 public:
  static void * TimerThreadFunc(void *arg);

 private:
  ImgConfigMgr *p_img_config_mgr_;
  EncDec enc_dec_;

  /// 统计计数
  int64_t all_req_count_;
  int64_t success_req_count_;
};
}}};  ///< end of namespace ganji::crawler::phone2image

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_ENC_URL_H_
