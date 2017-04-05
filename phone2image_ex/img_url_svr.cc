/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/img_url.cc
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  miaoxijun
 * @date    2011-09-18
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <fcgi_config.h>
#include <fcgiapp.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <map>
#include <string>

#include "util/log/thread_fast_log.h"
#include "util/thread/condition.h"
#include "util/thread/thread.h"
#include "util/net/http_header.h"
#include "util/thread/mutex.h"

#include "phone2image_ex/img_config.h"
#include "phone2image_ex/enc_url.h"
#include "phone2image_ex/img_util.h"


using std::map;
using std::string;
using ganji::crawler::phone2image::ImgConfigMgr;
using ganji::crawler::phone2image::EncUrl;
using ganji::util::thread::Condition;
using ganji::util::thread::Thread;

namespace FastLog = ganji::util::log::ThreadFastLog;
using FastLog::FastLogStat;
using FastLog::kLogSizeSplit;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogNotice;
using FastLog::kLogNone;
using FastLog::kLogAll;

static Condition exit_condition;
static int32_t fcgi_number = 0;
static Thread **fcgi_thread_array;

static ganji::util::thread::Mutex fcgi_mutex_;
static ImgConfigMgr g_img_config_mgr;

std::string http_header = "Status: 200\r\nContent-Type: text/html;\r\n\r\n";
std::string httpheader_error = "Status: 503\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
std::string update_header = "Status: 200\r\nContent-Type: text/html\r\n\r\n";
std::string update_header_error = "Status: 503\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
std::string update_body = "<html>update OK</html>";
std::string update_body_error = "<html>update failed</html>";

int PrepareServer(const string &conf_path) {
  srandom(time(NULL));

  if (g_img_config_mgr.Init(conf_path) < 0) {
    WriteLog(kLogFatal, "ImgConfigMgr Init failed");
    return NULL;
  }

  fcgi_number = g_img_config_mgr.GetThreadNum();
  fcgi_thread_array = new Thread*[fcgi_number];
  for (int i = 0; i < fcgi_number; ++i)
    fcgi_thread_array[i] = NULL;
  return 0;
}

void ReleaseServer() {
}

void SigTerm(int x) {
  exit_condition.Signal();
}

void SigUser1(int sig_no) {
  int ret = g_img_config_mgr.UpdateConf();
  if (ret == 0) {
    WriteLog(kLogNotice, "proc:%d UpdateConf OK", getpid());
  } else {
    WriteLog(kLogNotice, "proc:%d UpdateConf failed", getpid());
  }
}

void * FCGIMainLoop(void *arg) {
  FCGX_Request request;
  FCGX_InitRequest(&request, 0, 0);
  EncUrl enc_url;
  map<string, string> args_map;

  if (enc_url.Init(&g_img_config_mgr) < 0) {
    WriteLog(kLogFatal, "EncDec Init failed");
    return NULL;
  }

  srand(time(NULL));
  while (true) {
    // fcgi_mutex_.Lock();
    FCGX_Accept_r(&request);
    // fcgi_mutex_.Unlock();

    /// parse http arguments, include get & post arguments
    args_map.clear();
    char * query_str = FCGX_GetParam("QUERY_STRING", request.envp);
    if (query_str) {
      ganji::util::net::HttpHeaderArray::ParseHttpQuery(query_str, &args_map);
    }

    std::string image_id;
    /// 默认生成号码图片url
    if (enc_url.GenImgUrl(request.envp, query_str, args_map, &image_id) == 0) {
      FCGX_PutStr(http_header.c_str(), http_header.size(), request.out);
      FCGX_PutStr(image_id.c_str(), image_id.size(), request.out);
    } else {
      FCGX_PutStr(httpheader_error.c_str(), httpheader_error.size(), request.out);
    }

    FCGX_Finish_r(&request);
  }

  return NULL;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Usage:%s conf_path log_path\n", argv[0]);
    return 1;
  }

  string conf_path = argv[1];
  string log_path = argv[2];

  static FastLogStat log_st = {kLogAll, kLogFatal, kLogSizeSplit};
  FastLog::OpenLog(log_path.c_str(), "url", 1024 * 1024 * 1024, &log_st);

  FastLog::WriteLog(kLogNone, "img_url start");

  /// 处理信号USR1，用于load配置
  signal(SIGUSR1, SigUser1);
  signal(SIGTERM, SigTerm);
  signal(SIGINT, SigTerm);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);


  srand(time(0));
  char sev[] = "TZ=Asia/Shanghai";
  putenv(sev);
  ::tzset();

  if (PrepareServer(conf_path) < 0) {
    FastLog::WriteLog(kLogFatal, "PrepareServer faild");
    return -1;
  }

  FCGX_Init();

  for (int32_t i = 0; i < fcgi_number; ++i) {
    fcgi_thread_array[i] = Thread::CreateThread(FCGIMainLoop, 0);
    if (!fcgi_thread_array[i])
      return -4;
    fcgi_thread_array[i]->DetachThread();
    fcgi_thread_array[i]->ResumeThread();
  }

  exit_condition.Wait();

  ReleaseServer();

  FastLog::WriteLog(kLogNone, "img_url stop");
  /// FastLog::CloseLog(0);

  return 0;
}

