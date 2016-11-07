/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/downloader/downloader.h
 * @namespace ganji::crawler::conf_crawler::downloader
 * @version 1.0
 * @author  lisizhong
 * @date    2012-02-26
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_DOWNLOADER_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_DOWNLOADER_H_

#include <string>
#include <vector>
#include <queue>
#include <deque>

#include "global.h"
#include "util/thread/thread.h"
#include "util/thread/mutex.h"
#include "util/thread/condition.h"

#include "struct_def.h"
#include "conf_crawler/common/long_short_conn.h"
#include "conf_crawler/dc/DCService.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace downloader {
using std::string;
using std::vector;
using std::queue;
using std::deque;

using ganji::util::thread::Mutex;
using ganji::util::thread::Condition;
namespace thread = ganji::util::thread;
using ganji::crawler::conf_crawler::LongShortConn;

class DownloaderConfig;
class CurlDownloader;
class NetChecker;

typedef thread::Thread *ThreadPtr;
class Downloader;
struct ThreadArg {
  ThreadArg()
    : p_downloader(NULL),
    idx(0) {
  }
  ThreadArg(Downloader *my_downloader,
      int my_idx)
    : p_downloader(my_downloader),
    idx(my_idx) {
  }

  Downloader *p_downloader;
  int idx;
};

/**
 * @class Downloader
 * @brief 管理一个downloader与dc间的连接和下载需求
 */
class Downloader {
 public:
  Downloader() 
    : p_config_(NULL),
    p_get_task_thread_(NULL),
    p_upload_body_thread_(NULL),
    p_download_thread_(NULL),
    p_download_thread_arg_(NULL),
    p_downloader_(NULL),
    p_net_checker_(NULL) {
  }

  ~Downloader();

  int Init(DownloaderConfig *p_config);

  /// @brief 一直运行
  void Run();

 private:
  /// @brief timer thread
  static void * TimerThread(void *arg);
  int TimerThreadFunc();

  /// @brief 获取下载task线程
  static void * GetTaskThread(void *arg);
  int GetTaskThreadFunc();

  /// @brief 处理DC上传的body
  static void * UploadBodyThread(void *arg);
  int UploadBodyThreadFunc();

  /// @brief real download
  static void * DownloadThread(void *arg);
  int DownloadThreadFunc(int idx);

 private:
  /// @brief 上传给dc
  int UploadBody(const DownloadedBodyItem &downloaded_body_item);

 private:
  DownloaderConfig *p_config_; 

  LongShortConn<DCServiceClient> dc_conn_;  ///< 与dc的连接

  thread::Thread *p_timer_thread_;  ///< timer thread
  thread::Thread *p_get_task_thread_;  ///< get task thread
  thread::Thread *p_upload_body_thread_; ///< 接受downloader上传的body
  thread::Thread **p_download_thread_; ///< curl download thread

  deque<DownloadTask> download_queue_;    ///< queue for downloading task
  Mutex download_lock_;
  Condition download_cond_;

  /// serialize friendly download requests
  FriendlyQueue friendly_queue_;
  Domain2TimeMap domain_time_map_;
  Mutex friendly_lock_;

  queue<DownloadedBodyItem> upload_queue_;  ///< upload body item queue
  Mutex upload_lock_;  ///< for queue
  Condition upload_cond_;  ///< for queue

  ThreadArg *p_download_thread_arg_;           ///< argument for download thread
  CurlDownloader *p_downloader_;      ///< one for each downloader thread
  NetChecker *p_net_checker_;          ///< net checker

 private:
  DISALLOW_COPY_AND_ASSIGN(Downloader);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::downloader

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DOWNLOADER_DOWNLOADER_H_
