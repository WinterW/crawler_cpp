/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/phone_img_svr.cc
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

#include "phone2image_ex/img_builder.h"
#include "phone2image_ex/img_config.h"


using std::map;
using std::string;
using ganji::crawler::phone2image::ImgConfigMgr;
using ganji::crawler::phone2image::ImgBuilder;
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
static int32_t fcgi_number = 4;
static Thread **fcgi_thread_array;

static ganji::util::thread::Mutex fcgi_mutex_;
static ImgConfigMgr g_img_config_mgr;

const char kHttpHeader[] = "Status: 200 OK\r\nContent-Type: image/png;\r\n\r\n";
const char kHttpHeaderError[] = "Status: 503\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
const char kUpdateHeader[] = "Status: 200 OK\r\nContent-Type: text/html\r\n\r\n";
const char kUpdateHeaderError[] = "Status: 503\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
const char UpdateBody[] = "<html>update OK</html>";
const char kUpdateBodyError[] = "<html>update failed</html>";

int PrepareServer(const string &conf_path) {
  srandom(time(NULL));

  if (g_img_config_mgr.Init(conf_path) < 0) {
    WriteLog(kLogFatal, "ImgConfigMgr Init failed");
    return -1;
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
  ImgBuilder img_builder;
  map<string, string> args_map;

  if (img_builder.Init(&g_img_config_mgr) < 0) {
    WriteLog(kLogFatal, "ImgBuilder Init failed");
    return NULL;
  }

  srand(time(NULL));
  while (true) {
    // fcgi_mutex_.Lock();
    FCGX_Accept_r(&request);
    // fcgi_mutex_.Unlock();

    /// parse http arguments, include get & post arguments
    args_map.clear();
    char *query_string = FCGX_GetParam("QUERY_STRING", request.envp);
    if (query_string) {
      ganji::util::net::HttpHeaderArray::ParseHttpQuery(query_string, &args_map);
    }

    /*char *request_uri= FCGX_GetParam("REQUEST_URI", request.envp);
    string prefix=g_img_config_mgr.GetUrlPrefix();
    string uri= request_uri;
    string image_id = uri.substr(prefix.length()+1);*/

    string image_id = args_map["c"];
    string image_str;

    /// 默认生成号码图片请求
    if (img_builder.Request(request.envp, image_id, args_map, &image_str) == 0) {
      FCGX_PutStr(kHttpHeader, strlen(kHttpHeader), request.out);
      FCGX_PutStr(image_str.c_str(), image_str.size(), request.out);
    } else {
      FCGX_PutStr(kHttpHeaderError, strlen(kHttpHeaderError), request.out);
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
  FastLog::OpenLog(log_path.c_str(), "phone_image_svr", 1024 * 1024 * 1024, &log_st);

  FastLog::WriteLog(kLogNone, "phone_image_svr start");

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

  FastLog::WriteLog(kLogNone, "phone_image_svr stop");
  // FastLog::CloseLog(0);

  return 0;
}

