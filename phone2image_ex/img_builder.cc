/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/image_build.cc
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
#include <unistd.h>
#include "phone2image_ex/img_builder.h"

#include <stdio.h>

#include <fcgi_config.h>
#include <fcgiapp.h>

#include "util/log/thread_fast_log.h"
#include "util/time/time.h"
#include "util/thread/sleep.h"
#include "util/thread/thread.h"
#include "util/text/text.h"
//#include "ganji/crawler/phone2image_ex/Pass.h"

#include "phone2image_ex/des_decoder.h"
#include "phone2image_ex/phone_img.h"
#include "phone2image_ex/img_config.h"
#include "phone2image_ex/img_util.h"

namespace ganji { namespace crawler { namespace phone2image {
using std::map;
using std::string;

/*
using apache::thrift::transport::TSocket;
using apache::thrift::transport::TFramedTransport;
using apache::thrift::protocol::TBinaryProtocol;
using boost::shared_ptr;
*/

namespace FastLog = ganji::util::log::ThreadFastLog;
using FastLog::kLogFatal;
using FastLog::kLogNotice;
using FastLog::WriteLog;
using FastLog::kLogWarning;
namespace Time = ::ganji::util::time;
using ganji::util::thread::Thread;
namespace Text=ganji::util::text::Text;
namespace Sleep = ::ganji::util::thread::Sleep;

ImgBuilder::ImgBuilder()
  : p_img_config_mgr_(NULL) {
  p_decoder_ = new DesDecoder();
  p_phone_image_ = new PhoneImg();
}

ImgBuilder::~ImgBuilder() {
  if (p_decoder_)
    delete p_decoder_;
  if (p_phone_image_)
    delete p_phone_image_;
}

int ImgBuilder::Init(ImgConfigMgr *p_img_config_mgr) {
  all_req_count_ = 0;
  success_req_count_ = 0;

  p_img_config_mgr_ = p_img_config_mgr;

  int ret = p_decoder_->Init(p_img_config_mgr);
  if (ret < 0) {
    WriteLog(kLogFatal, "Decoder Init failed");
    return -1;
  }

  ret = p_phone_image_->Init(p_img_config_mgr);
  if (ret < 0) {
    WriteLog(kLogFatal, "Phone2Image Init failed");
    return -1;
  }

  Thread *t = Thread::CreateThread(TimerThreadFunc, this);
  if (!t) {
    WriteLog(kLogFatal, "create timer thread failed:%s", strerror(errno));
    return -1;
  }
  t->DetachThread();
  t->ResumeThread();

  return 0;
}

int ImgBuilder::QueryLevel(char **p_env,
                           const string &source_ip,
                           const string &domain,
                           const string &cat_major,
                           int *p_level) {
  *p_level = 0;
/*
  TSocket *p_sock = NULL;
  TFramedTransport *p_trans = NULL;
  TBinaryProtocol *p_prot = NULL;
  PassClient *p_client = NULL;

  const string &anti_spider_ip = p_img_config_mgr_->GetAntiSpiderConfig()->ip_;
  int anti_spider_port = p_img_config_mgr_->GetAntiSpiderConfig()->port_;
  int sock_time_out = p_img_config_mgr_->GetAntiSpiderConfig()->time_out_;
  bool is_success = true;

  try {
    p_sock = new TSocket(anti_spider_ip, anti_spider_port);
    p_sock->setConnTimeout(sock_time_out);
    p_sock->setRecvTimeout(sock_time_out);
    p_sock->setSendTimeout(sock_time_out);
    p_sock->open();
    shared_ptr<TSocket> sock_ptr(p_sock);

    p_trans = new TFramedTransport(sock_ptr);
    shared_ptr<TFramedTransport> trans_ptr(p_trans);

    p_prot = new TBinaryProtocol(trans_ptr);
    shared_ptr<TBinaryProtocol> prot_ptr(p_prot);

    p_client = new PassClient(prot_ptr);

    RequestItem request_item;

    request_item.ip = source_ip;

    request_item.domain = domain;
    Time::GetY4MDHMS2(time(NULL), &request_item.time_str);
    char *req_method = FCGX_GetParam("REQUEST_METHOD", p_env);
    if (req_method)
      request_item.req_method = req_method;
    char *req_str = FCGX_GetParam("REQUEST_URI", p_env);
    if (req_str)
      request_item.req_str = req_str;
    char *referer = FCGX_GetParam("HTTP_REFERER", p_env);
    if (referer)
      request_item.referer = referer;
    char *user_agent = FCGX_GetParam("HTTP_USER_AGENT", p_env);
    if (user_agent)
      request_item.user_agent = user_agent;
    request_item.category_major = cat_major;
    *p_level = p_client->process_request(request_item);
    p_trans->close();
  } catch(...) {
    is_success = false;
  }

  delete p_client;
  

  if (is_success)
    return 0;
  else*/
    return -1;
}

int ImgBuilder::Request(char **p_env, const string &image_id, const map<string, string> &args_map, string *image_str) {
  all_req_count_++;

  string x_forwarded_for, source_ip;
  int ret = ImgUtil::GetSourceIp(p_env, &x_forwarded_for, &source_ip);
  if (ret < 0) {
    WriteLog(kLogWarning, "Request() failed, no source ip");
    return -1;
  }
  /// 不允许原始ip为内网
  bool exclude_inner_ip = p_img_config_mgr_->GetExcludeInnerIp();
  if (exclude_inner_ip) {
    if (ImgUtil::IsValidInnerIp(source_ip)) {
      WriteLog(kLogWarning, "Request() failed, inner source ip:%s", source_ip.c_str());
      return -1;
    }
  }

  /// 解码url，获取各参数
  if (p_decoder_->Decode(image_id, args_map) < 0) {
    WriteLog(kLogWarning, "ip:[%s] decode:%s failed -- %s",
             x_forwarded_for.c_str(), image_id.c_str(), p_decoder_->GetErrBuf());
    return -1;
  }

  const string &domain = p_decoder_->GetDomain();
  const string &cat_major = p_decoder_->GetCatMajor();
  string phone = p_decoder_->GetPhone();
  Text::Trim(&phone);
  if(phone.length() <= 0) return -1;
  const string &style = p_decoder_->GetStyle();


  /// 获取ip对应的封禁级别
  int level = p_decoder_->GetLevel();

  /// 根据各参数，生成图片
  ret = p_phone_image_->PhoneToImage(level,
                                     style,
                                     phone,
                                     image_str);
  if (ret < 0) {
    WriteLog(kLogWarning, "Request() ip:[%s] PhoneToImage failed",
             x_forwarded_for.c_str());
  } else {
    success_req_count_++;
  }

  return ret;
}

void *ImgBuilder::TimerThreadFunc(void *arg) {
  ImgBuilder *p_this = reinterpret_cast<ImgBuilder *>(arg);
  if (!p_this) {
    return NULL;
  }

  while (true) {
    int time_interval = p_this->p_img_config_mgr_->GetTimeInterval() * 1000;
    Sleep::DoSleep(time_interval);

    int64_t all_req_count = p_this->all_req_count_;
    int64_t success_req_count = p_this->success_req_count_;
    int64_t failed_req_count = all_req_count - success_req_count;

    pid_t pid = getpid();
    WriteLog(kLogNotice, "proc:%d all req count:%ld", pid, all_req_count);
    WriteLog(kLogNotice, "proc:%d success req count:%ld", pid, success_req_count);
    WriteLog(kLogNotice, "proc:%d failed req count:%ld", pid, failed_req_count);

    p_this->all_req_count_ = 0;
    p_this->success_req_count_ = 0;
  }

  return NULL;
}
}}}
