/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/img_util.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  lisizhong
 * @date    2011-12-23
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_UTIL_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_UTIL_H_

#include <string>

struct FCGX_Request;
namespace ganji { namespace crawler { namespace phone2image { namespace ImgUtil {
  int GetXForwardedFor(char **p_env, std::string *p_x_forwarded_for);

  int GetRemoteAddr(char **p_env, std::string *p_remote_addr);

  int GetSourceIp(char **p_env, std::string *p_x_forwarded_for, std::string *p_source_ip);

  /// @brief ip是否为内网
  bool IsValidInnerIp(const std::string &ip);

  void ShowEnv(FCGX_Request *p_request);
}}}};  ///< end of namespace ganji::crawler::phone2image::ImgUtil

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_UTIL_H_
