/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/dc/dc.h
 * @namespace ganji::crawler::conf_crawler::dc
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_DC_DC_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_DC_DC_H_

#include <string>
#include <vector>
#include <queue>

#include "global.h"
#include "util/thread/thread.h"
#include "util/thread/mutex.h"
#include "util/thread/rwlock.h"
#include "util/thread/condition.h"

#include "conf_crawler/common/long_short_conn.h"
#include "conf_crawler/dc/LinkBaseService.h"
#include "conf_crawler/dc/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace dc {
using ganji::util::thread::Mutex;
using ganji::util::thread::Condition;
using ganji::util::thread::RWLock;
using ganji::util::thread::Thread;
using ganji::crawler::conf_crawler::LongShortConn;

class DcConfig;
typedef Thread *ThreadPtr;

/**
 * @class Dc
 * @brief downloader center, manage all download request/response between link base and downloaders
 */
class Dc {
 public:
  Dc()
    : p_config_(NULL),
    p_timer_thread_(NULL),
    p_proc_upload_task_thread_(NULL),
    p_upload_body_thread_(NULL) {
  }

  ~Dc();

  int Init(DcConfig *p_config);

  /// @brief distribute task for downloader
  int OnGetTask(std::vector<DownloadTask> *p_list);

  /// push task item
  void PushTaskItem(const DownloadTaskItem &task_item);

  /// @brief process download task uploaded by downloader
  int OnUploadTask(const DownloadedBodyItem &downloaded_body_item);

  /// push delay task queue to download queue
  void DelayTaskPush();

 private:
  static void *TimerThread(void *arg);
  /// @brief process expired request
  void ExpireRequest();

  /// @brief process body uploaded by dc
  static void *ProcUploadTaskThread(void *arg);
  int ProcUploadTaskThreadFunc();

  /// @brief upload body to link base
  static void *UploadBodyThread(void *arg);
  int UploadBodyThreadFunc();
  int UploadBody(const DownloadedBodyItem &downloaded_body_item);

 private:
  DcConfig *p_config_;

  /// connection with link base
  LongShortConn<LinkBaseServiceClient> link_base_conn_;

  /// threads
  Thread *p_timer_thread_;
  Thread *p_proc_upload_task_thread_;
  Thread *p_upload_body_thread_;

  /// request task to download
  DownloadTaskQueue download_queue_;
  Mutex download_lock_;

  /// request delay task to re download
  DownloadTaskQueue delay_download_queue_;
  Mutex delay_download_lock_;

  /// request task downloading
  DownloadTaskMap download_map_;
  Mutex cache_lock_;

  /// upload task queue
  std::queue<DownloadedBodyItem> upload_task_queue_;
  Mutex upload_task_lock_;
  Condition upload_task_cond_;

  /// downloaded body item queue
  std::queue<DownloadedBodyItem> downloaded_queue_;
  Mutex downloaded_lock_;
  Condition downloaded_cond_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Dc);
};

/**
 * @class DcManager
 * @brief dc manager, manage each type of dc, e.g. normal dc & webkit dc
 */
class DcManager {
 public:
  DcManager()
    : p_config_(NULL),
    p_get_task_thread_(NULL) {
  }

  ~DcManager();

  int Init(DcConfig *p_config);

  /// @brief distribute task for downloader
  int OnGetTask(std::vector<DownloadTask> *p_list, const DownloaderType::type downloader_type);

  /// push task by client
  void OnPushTask(const DownloadTask &download_task);

  /// @brief process download task uploaded by downloader
  int OnUploadTask(const DownloadedBodyItem &downloaded_body_item);

 private:
  /// @brief get task from link base
  static void *GetTaskThread(void *arg);
  int GetTaskThreadFunc();

  int FillTaskItem(DownloadTaskItem *p_task_item);

  /// @brief get ip by url
  /// @param[in] url the url to be processed
  /// @param[out] p_ip the ip for the url
  /// @return 0:success -1:failure
  int GetUrlIp(const std::string &url, std::string *p_ip);

  /// @brief DNS lookup
  static int DnsLookup(const std::string &domain, std::string *p_ip);

 private:
  DcConfig *p_config_;

  LongShortConn<LinkBaseServiceClient> link_base_conn_;  ///< connection with link base

  Dc normal_dc_;      ///< dc for normal download
  Dc webkit_dc_;      ///< dc for webkit download

  /// threads
  Thread **p_get_task_thread_;

  /// DNS cache, domain => ip
  DnsCacheMap dns_cache_map_;
  RWLock domain_ip_lock_;

 private:
  DISALLOW_COPY_AND_ASSIGN(DcManager);
};
}}}};  ///< end of namespace ganji::crawler::conf_crawler::dc

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_DC_DC_H_
