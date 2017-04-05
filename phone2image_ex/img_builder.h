/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/img_builder.h
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_BUILDER_H_
#define _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_BUILDER_H_

#include <string.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <vector>

namespace ganji { namespace crawler { namespace phone2image {
class DesDecoder;
class PhoneImg;
class ImgConfigMgr;

/**
 * @class ImgBuilder
 * @brief 生成图片
 */
class ImgBuilder {
 public:
  ImgBuilder();

  ~ImgBuilder();

  int Init(ImgConfigMgr *p_img_config_mgr);

  /// @brief 根据输入的图片参数生成图片
  /// @param[in] p_env fcgi的输入参数
  /// @param[in] args_map 参数p_env对应的列表
  /// @param[out] image_str 生成的图片二进制数据
  /// @return 0:success -1:failure
  int Request(char **p_env, const std::string &image_id, const std::map<std::string, std::string> &args_map, std::string *image_str);

 public:
  static void * TimerThreadFunc(void *arg);

 private:
  /// @brief 向防抓取后台服务查询ip对应的封禁级别
  /// @param[in] p_env fcgi的输入参数
  /// @param[in] source_ip 原始客户端的ip
  /// @param[in] domain 请求的域名
  /// @param[in] cat_major 一级小类的id
  /// @param[out] p_level 封禁级别，默认为0
  /// @return 0:success -1:failure
  int QueryLevel(char **p_env,
                 const std::string &source_ip,
                 const std::string &domain,
                 const std::string &cat_major,
                 int *p_level);

 private:
  ImgConfigMgr *p_img_config_mgr_;
  DesDecoder *p_decoder_;
  /// 实际的生成图片类
  PhoneImg *p_phone_image_;

  /// 统计计数
  int64_t all_req_count_;
  int64_t success_req_count_;
};
}}}

#endif  ///< _GANJI_CRAWLER_PHONE2IMAGE_EX_IMG_BUILDER_H_
