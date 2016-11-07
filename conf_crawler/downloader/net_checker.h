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

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_NET_CHECKER_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_NET_CHECKER_H_

#include <string>
#include <vector>

#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "global.h"
#include "util/thread/thread.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
using ganji::util::thread::Thread;
typedef Thread *ThreadPtr;

class DownloaderConfig;
class NetChecker;
class CurlDownloader;


/**
 * @struct CheckThreadArg
 * @brief argument for CheckThread
 */
struct CheckThreadArg {
  CheckThreadArg()
    : p_checker(NULL),
    idx(-1) {
  }
  CheckThreadArg(NetChecker *checker,
      int thread_idx)
    : p_checker(checker),
    idx(thread_idx) {
  }

  NetChecker *p_checker;
  int idx;
};

/**
 * @class NetChecker 
 * @brief check whether local interface can connect to outer network
 */
class NetChecker {
 public:
  NetChecker() 
    : p_config_(NULL),
    p_check_thread_(NULL),
    p_check_ip_thread_(NULL),
    p_check_thread_arg_(NULL),
    total_(0),
    success_(0),
    is_net_ok_(true) {
  }

  ~NetChecker();

  int Init(DownloaderConfig *p_config);

  bool IsNetOk() { return is_net_ok_; }

 private:
  /// @brief check thread
  static void * CheckThread(void *arg);
  int CheckThreadFunc();

  /// @brief check domain thread
  static void * CheckDomainThread(void *arg);
  int CheckDomainThreadFunc(int idx);

  /// @brief DNS lookup
  int DnsLookup(const std::string &domain, std::string *p_ip);

  /// @brief convert milliseconds into timeval
  void CnvtMs2Timeval(const int msec, struct timeval *ptv);

  /// @brief encapsulate select()
  int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

  /// @brief wait until timeout or fd is readable/writable
  int SRWableMs(int fd, int mseconds);

 private:
  DownloaderConfig *p_config_; 

  std::vector<std::string> ip_list_;

  ///< check thread
  Thread *p_check_thread_;
  ///< ptr to check ip threads
  ThreadPtr *p_check_ip_thread_;
  ///< argument array for check thread
  CheckThreadArg *p_check_thread_arg_;

  int total_;
  int success_;
  ///< whether local interface can connect to outer network
  bool is_net_ok_;

 private:
  DISALLOW_COPY_AND_ASSIGN(NetChecker);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_NET_CHECKER_H_
