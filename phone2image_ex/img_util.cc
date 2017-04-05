/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image_ex/img_util.cc
 * @namespace ganji::crawler::phone2image
 * @version 1.0
 * @author  lisizhong
 * @date    2011-12-23
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "phone2image_ex/img_util.h"

#include <fcgiapp.h>
#include <string.h>
#include <limits.h>

#include "util/log/thread_fast_log.h"

using std::string;

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

namespace ganji { namespace crawler { namespace phone2image { namespace ImgUtil {
int GetXForwardedFor(char **p_env, string *p_x_forwarded_for) {
  char *x_forwarded_for = FCGX_GetParam("X_FORWARDED_FOR", p_env);
  if (!x_forwarded_for) {
    WriteLog(kLogFatal, "no X_FORWARDED_FOR in env");
    return -1;
  }
  *p_x_forwarded_for = x_forwarded_for;

  if (p_x_forwarded_for->empty()) {
    WriteLog(kLogFatal, "X_FORWARDED_FOR empty");
    return -1;
  }

  return 0;
}

int GetRemoteAddr(char **p_env, string *p_remote_addr) {
  char *remote_addr = FCGX_GetParam("REMOTE_ADDR", p_env);
  if (!remote_addr) {
    WriteLog(kLogFatal, "no REMOTE_ADDR in env");
    return -1;
  }
  *p_remote_addr = remote_addr;

  if (p_remote_addr->empty()) {
    WriteLog(kLogFatal, "REMOTE_ADDR empty");
    return -1;
  }

  return 0;
}

int GetSourceIp(char **p_env, string *p_x_forwarded_for, string *p_source_ip) {
  p_source_ip->clear();
  int ret = GetXForwardedFor(p_env, p_x_forwarded_for);
  if (ret == 0) {
    *p_source_ip = *p_x_forwarded_for;
    size_t pos = p_x_forwarded_for->find(",");
    if (pos != string::npos) {
      *p_source_ip = p_x_forwarded_for->substr(0, pos);
    }
  }
  if (p_source_ip->empty()) {
    WriteLog(kLogFatal, "invalid X_FORWARDED_FOR:%s", p_x_forwarded_for->c_str());
    ret = GetRemoteAddr(p_env, p_source_ip);
    if (ret < 0)
      return -1;
  }

  return 0;
}

bool IsValidInnerIp(const string &ip) {
  if (ip.empty()) {
    return false;
  }
  /// 内网IP
  const char inner_ip1[] = "192.168.";
  if (strncmp(ip.c_str(), inner_ip1, sizeof(inner_ip1)-1) == 0) {
    return true;
  }
  const char inner_ip2[] = "10.";
  if (strncmp(ip.c_str(), inner_ip2, sizeof(inner_ip2)-1) == 0) {
    return true;
  }
  const char inner_ip3[] = "172.";
  if (strncmp(ip.c_str(), inner_ip3, sizeof(inner_ip3)-1) == 0) {
    const char *p = ip.c_str() + sizeof(inner_ip3) - 1;
    if (isdigit(*p) == 0)
      return false;
    int64_t seg_val = strtol(p, NULL, 10);
    if (seg_val == LONG_MIN || seg_val == LONG_MAX)
      return false;
    if (seg_val >= 16 && seg_val <= 31)
      return true;
    return false;
  }

  if (ip == "127.0.0.1")
    return true;

  return false;
}

void ShowEnv(FCGX_Request *p_request) {
  FCGX_Request &request = *p_request;
  const string br_str = "<br>\n";
  for (char **p = request.envp; *p != NULL; p++) {
    FCGX_PutStr(*p, strlen(*p), request.out);
    FCGX_PutStr(br_str.c_str(), br_str.size(), request.out);
  }
}
}}}};  ///< end of namespace ganji::crawler::phone2image::ImgUtil
