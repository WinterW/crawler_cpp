/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/net_checker.h
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-03-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "net_checker.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "downloader_config.h"

#include "util/text/text.h"
#include "util/time/time.h"
#include "util/thread/sleep.h"
#include "util/net/http_opt.h"
#include "util/net/ip_num.h"
#include "util/system/system.h"
#include "util/log/thread_fast_log.h"

using std::string;
using std::vector;

namespace Http = ::ganji::util::net::Http;
namespace IpNum = ::ganji::util::net::IpNum;
namespace Text = ::ganji::util::text::Text;
namespace Sleep = ::ganji::util::thread::Sleep;
namespace Time = ganji::util::time;
namespace System = ::ganji::util::system::System;
using ganji::util::thread::Thread;
namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
NetChecker::~NetChecker() {
  Thread::FreeThread(p_check_thread_);

  if (p_check_ip_thread_) {
    for (size_t i = 0; i < ip_list_.size(); i++) {
      Thread::FreeThread(p_check_ip_thread_[i]);
    }
    delete []p_check_ip_thread_;
  }
  if (p_check_thread_arg_)
    delete []p_check_thread_arg_;
}

int NetChecker::Init(DownloaderConfig *p_config) {
  if (!p_config) {
    WriteLog(kLogFatal, "downloader config NULL");
    return -1;
  }

  p_config_ = p_config;

  total_ = 0;
  success_ = 0;
  is_net_ok_ = true;

  /// Dns Lookup to get ip list
  const vector<string> &domain_list = p_config_->GetDomainList();
  if (domain_list.empty()) {
    WriteLog(kLogFatal, "domain list empty");
    return -1;
  }
  ip_list_.clear();
  for (vector<string>::const_iterator it = domain_list.begin();
      it != domain_list.end(); ++it) {
    const string &domain = *it;
    string ip;
    if (DnsLookup(domain, &ip))
      return -1;
    ip_list_.push_back(ip);
    WriteLog(kLogNotice, "dns[%s:%s]", domain.c_str(), ip.c_str());
  }

  int ip_count = ip_list_.size();

  p_check_ip_thread_ = new ThreadPtr[ip_count];
  p_check_thread_arg_ = new CheckThreadArg[ip_count];
  for (int i = 0; i < ip_count; i++) {
    p_check_thread_arg_[i] = CheckThreadArg(this, i);
  }

  /// create check thread
  p_check_thread_ = Thread::CreateThread(CheckThread, this);
  p_check_thread_->ResumeThread();

  return 0;
}

void * NetChecker::CheckThread(void *arg) {
  NetChecker *p_checker = reinterpret_cast<NetChecker *>(arg);
  assert(p_checker);

  uint32_t last_t = Time::GetCurTimeMs();
  int interval = p_checker->p_config_->GetNetCheckInterval();
  while (true) {
    p_checker->CheckThreadFunc();
    uint32_t cur_t = Time::GetCurTimeMs();
    int elapsed_t = cur_t - last_t;
    if (elapsed_t < interval)
      Sleep::DoSleep(interval-elapsed_t);
    last_t = cur_t;
  }

  return NULL;
}

int NetChecker::CheckThreadFunc() {
  int ip_count = ip_list_.size();
  /// clear
  total_ = 0;
  success_ = 0;

  /// ptr to check ip threads
  for (int i = 0; i < ip_count; i++) {
    p_check_ip_thread_[i] = Thread::CreateThread(CheckDomainThread, &p_check_thread_arg_[i]);
    p_check_ip_thread_[i]->ResumeThread();
  }
  for (int i = 0; i < ip_count; i++) {
    p_check_ip_thread_[i]->WaitThread(0);
    Thread::FreeThread(p_check_ip_thread_[i]);
  }

  if (total_ <= 0)
    is_net_ok_ = false;
  else if (2 * success_ < total_)
    is_net_ok_ = false;
  else
    is_net_ok_ = true;
  WriteLog(kLogNotice, "net ok:%d", is_net_ok_);

  return 0;
}

void * NetChecker::CheckDomainThread(void *arg) {
  CheckThreadArg *p_arg = reinterpret_cast<CheckThreadArg *>(arg);
  NetChecker *p_checker = p_arg->p_checker;
  int idx = p_arg->idx;

  p_checker->CheckDomainThreadFunc(idx);

  return NULL;
}

int NetChecker::CheckDomainThreadFunc(int idx) {
  const vector<string> &domain_list = p_config_->GetDomainList();
  const string &domain = domain_list[idx];
  const string &ip = ip_list_[idx];
  int port = 80;
  int time_out = p_config_->GetNetCheckTimeout();
  const string &local_interface = p_config_->GetLocalInterface();

  total_++;

  /// create socket
  int conn_fd = 0;
  bool is_net_ok = true;
  do {
    if ((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      WriteLog(kLogFatal, "socket() failed:%s", strerror(errno));
      is_net_ok = false;
      break;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) < 0) {
      WriteLog(kLogFatal, "inet_pton() failed:%s", strerror(errno));
      is_net_ok = false;
      break;
    }

    int ret = 0;
    if (!local_interface.empty()) {
      struct sockaddr_in local_addr;
      memset(&local_addr, 0, sizeof(local_addr));
      local_addr.sin_family = AF_INET;
      if (inet_pton(AF_INET, local_interface.c_str(), &local_addr.sin_addr) < 0) {
        WriteLog(kLogFatal, "inet_pton() failed:%s", strerror(errno));
        is_net_ok = false;
        break;
      }

      /// bind
      if ((ret = bind(conn_fd, reinterpret_cast<const struct sockaddr *>(&local_addr), sizeof(local_addr))) < 0) {
        WriteLog(kLogFatal, "bind()[%s] failed:%s", local_interface.c_str(), strerror(errno));
        is_net_ok = false;
        break;
      }
    }

    /// set socket in nonblocking mode
    int flags = fcntl(conn_fd, F_GETFL, 0);
    if (flags < 0) {
      WriteLog(kLogFatal, "fcntl() failed:%s", strerror(errno));
      is_net_ok = false;
      break;
    }
    if (fcntl(conn_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
      WriteLog(kLogFatal, "fcntl() failed:%s", strerror(errno));
      is_net_ok = false;
      break;
    }

    /// connect
    if ( (ret = connect(conn_fd, reinterpret_cast<const struct sockaddr *>(&serv_addr), sizeof(serv_addr))) < 0 &&
        errno != EINPROGRESS) {
      WriteLog(kLogFatal, "connect() failed:%s", strerror(errno));
      is_net_ok = false;
      break;
    }

    /// connect completed immediately
    if (ret == 0) {
      break;
    } else {
      ret = SRWableMs(conn_fd, time_out);
      /// timeout or error, close socket
      if (ret == 0) {
        WriteLog(kLogFatal, "connect() timeout");
        is_net_ok = false;
        break;
      } else if (ret < 0) {
        WriteLog(kLogFatal, "RWable() failed");
        is_net_ok = false;
        break;
      } else {
        is_net_ok = true;
      }
    }
  } while (0);

  if (conn_fd > 0) {
    close(conn_fd);
  }

  if (is_net_ok) {
    WriteLog(kLogDebug, "check[%s:%s] OK", domain.c_str(), ip.c_str());
    success_++;
    return 0;
  }

  WriteLog(kLogWarning, "check[%s:%s] failed", domain.c_str(), ip.c_str());
  
  return 0;
}

int NetChecker::DnsLookup(const string &domain, string *p_ip) {
  char buf[1024];
  struct hostent host;
  struct hostent *p_host;
  int ret = gethostbyname_r(domain.c_str(),
      &host,
      buf,
      sizeof(buf),
      &p_host,
      &h_errno);

  if (ret != 0) {
    WriteLog(kLogFatal, "gethostbyname_r[%s] failed:%d", domain.c_str(), h_errno);
    return -1;
  }
  uint32_t ip_num = static_cast<uint32_t>((reinterpret_cast<struct in_addr*>(host.h_addr))->s_addr);
  ret = IpNum::Num2Ip(ip_num, p_ip);
  if (ret < 0) {
    WriteLog(kLogFatal, "Num2Ip failed:%s", strerror(errno));
    return -1;
  }
  return 0;
}

void NetChecker::CnvtMs2Timeval(const int msec, struct timeval *ptv) {
  ptv->tv_sec = msec /1000;
  ptv->tv_usec = (msec % 1000) * 1000;
}

int NetChecker::Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
 again:
  int val = select(nfds, readfds, writefds, exceptfds, timeout);
  if (val < 0) {
    if (errno == EINTR) {
      goto again;
    }
    WriteLog(kLogFatal, "select() failed:%s", strerror(errno));
  }

  return val;
}

int NetChecker::SRWableMs(int fd, int mseconds) {
  fd_set rset, wset;
  struct timeval tv;
  CnvtMs2Timeval(mseconds, &tv);
  FD_ZERO(&rset);
  FD_SET(fd, &rset);
  wset = rset;

  int ret = Select(fd+1, &rset, &wset, NULL, &tv);
  /// 超时
  if (ret == 0)
    return 0;
  /// select出错
  if (ret < 0)
    return -1;
  bool is_readable = FD_ISSET(fd, &rset);
  bool is_writable = FD_ISSET(fd, &wset);
  if (is_readable || is_writable) {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
      return -1;
    if (error) {
      return -1;
    }
  } else {
    WriteLog(kLogFatal, "select error: sockfd not set");
    return -1;
  }
  return ret;
}
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

