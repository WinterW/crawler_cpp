/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/enc_url.cc
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  lisizhong
 * @date    2011-11-04
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "phone2image_ex/enc_url.h"

#include <string>
#include <map>
#include <set>
#include <vector>

#include "util/log/thread_fast_log.h"
#include "util/thread/sleep.h"
#include "util/thread/thread.h"

#include "phone2image_ex/img_config.h"
#include "phone2image_ex/img_util.h"

namespace ganji { namespace crawler { namespace phone2image {
using std::string;
using std::map;
using ganji::util::thread::Thread;
namespace Sleep = ::ganji::util::thread::Sleep;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

int EncUrl::Init(ImgConfigMgr *p_img_config_mgr) {
  all_req_count_ = 0;
  success_req_count_ = 0;

  p_img_config_mgr_ = p_img_config_mgr;

  int ret = enc_dec_.Init(p_img_config_mgr);
  if (ret < 0) {
    WriteLog(kLogFatal, "EncUrl Init failed");
  }

  Thread *t = Thread::CreateThread(TimerThreadFunc, this);
  if (!t) {
    WriteLog(kLogFatal, "create timer thread failed:%s", strerror(errno));
    return -1;
  }
  t->DetachThread();
  t->ResumeThread();

  return ret;
}

int EncUrl::GenImgUrl(char **p_env,
                      const char *query_str,
                      const map<string, string> &args_map,
                      string *p_img_id) {
  all_req_count_++;

  /// 获取来源的ip
  string x_forwarded_for, source_ip;
  int ret = ImgUtil::GetSourceIp(p_env, &x_forwarded_for, &source_ip);
  if (ret < 0) {
    WriteLog(kLogFatal, "GenImgUrl() failed, no source ip");
    return -1;
  }

  string phone, style, domain, cat_major;
  int level = 0;
  time_t cur_time = 0;

  /// 从args_map中解析出各参数，并进行合法性判断
  map<string, string>::const_iterator it_phone = args_map.find(kPhone);
  if (it_phone == args_map.end()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] no phone:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }
  phone = it_phone->second;
  if (phone.empty()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] phone empty:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }

  map<string, string>::const_iterator it_style = args_map.find(kStyle);
  /// XXX 无法解析出style，设定为默认style
  if (it_style == args_map.end()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] no style:%s use default", x_forwarded_for.c_str(), query_str);
    style = kDefaultStyle;
  } else {
    style = it_style->second;
  }
  if (style.empty()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] style empty:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }

  map<string, string>::const_iterator it_domain = args_map.find(kDomain);
  if (it_domain == args_map.end()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] no domain:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }
  domain = it_domain->second;
  if (domain.empty()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] domain empty:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }

  map<string, string>::const_iterator it_cat_major = args_map.find(kCatMajor);
  if (it_cat_major == args_map.end()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] no cat_major:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }
  cat_major = it_cat_major->second;
  if (cat_major.empty()) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] cat_major empty:%s", x_forwarded_for.c_str(), query_str);
    return -1;
  }

  /// 对各参数进行处理并加密
  ret = enc_dec_.EncodeParams(phone,
                              style,
                              domain,
                              cat_major,
                              level,
                              cur_time,
                              p_img_id);
  if (ret < 0) {
    WriteLog(kLogFatal, "GenImgUrl() failed, ip:[%s] enc_dec error:%s",
             x_forwarded_for.c_str(), enc_dec_.GetErrBuf());
  } else {
    /// 加上前缀
    *p_img_id = "http://" + domain + "/" + p_img_config_mgr_->GetUrlPrefix() + *p_img_id;
    success_req_count_++;
  }

  return ret;
}

void *EncUrl::TimerThreadFunc(void *arg) {
  EncUrl *p_enc_url = reinterpret_cast<EncUrl *>(arg);
  if (!p_enc_url) {
    return NULL;
  }

  while (true) {
    int time_interval = p_enc_url->p_img_config_mgr_->GetTimeInterval() * 1000;
    Sleep::DoSleep(time_interval);

    int64_t all_req_count = p_enc_url->all_req_count_;
    int64_t success_req_count = p_enc_url->success_req_count_;
    int64_t failed_req_count = all_req_count - success_req_count;

    pid_t pid = getpid();
    WriteLog(kLogNotice, "proc:%d all req count:%ld", pid, all_req_count);
    WriteLog(kLogNotice, "proc:%d success req count:%ld", pid, success_req_count);
    WriteLog(kLogNotice, "proc:%d failed req count:%ld", pid, failed_req_count);

    p_enc_url->all_req_count_ = 0;
    p_enc_url->success_req_count_ = 0;
  }

  return NULL;
}
}}};  ///< end of namespace ganji::crawler::phone2image
