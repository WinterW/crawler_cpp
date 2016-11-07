/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file          ganji/util/log/thread_fast_log.h
 * @namespace     ganji::util::log
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * fast log
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 * 日志的使用如下
 * 
 * 多线程程序:
 *
 * 主进程              子线程 1         子线程 2 ...
 *
 *  FastLog::OpenLog
 *      |
 *      ------------------------------------------
 *      |                      |                 |
 *  FastLog::WriteLog  FastLog::ThreadOpenLog  FastLog::ThreadOpenLog
 *     ...                     |                 |
 *     ...             FastLog::WriteLog  FastLog::WriteLog
 *     ...                    ...               ...
 *                     FastLog::ThreadCloseLog FastLog::ThreadCloseLog
 *                            |                  |
 *    --------------------------------------------
 *      |
 *  FastLog::CloseLog
 *
 *  如果在FastLog::OpenLog 前调用 FastLog::WriteLog
 *  日志信息会被输出到标准出错(stderr)上,日志库支持多线程呈现,但不支持多进程程序,
 *  多进程中使用请保证每个进程使用的是不用的日志文件
 */

#ifndef _GANJI_UTIL_LOG_FAST_LOG_H_
#define _GANJI_UTIL_LOG_FAST_LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include "global.h"

namespace ganji { namespace util { namespace log { namespace ThreadFastLog {

const int32_t kLogFileNumber = 2048;          ///< 最多可同时打开的文件数
const int32_t kMaxLogBuffSize = 2048;         ///< 一条日志buff最大字节数
const int32_t kMaxLogFileSize = 2045;         ///< 最大文件大小单位(MB)
const int32_t kMaxLogFileNameLength = 1024;   ///< 日志文件路径的最大长度

const int32_t kMaxSelfDefineLog = 8;          ///< 单个线中自定义日志的日志数上限
const int32_t kNameNumberInData = 100;
const int32_t kNameLengthInData = 32;
const int32_t kBufferSizeInData = 640;

const int32_t kProcNameLength = 256;
const int32_t kTimeLenght = 20;


/// 错误类型
enum {
  kErrorNoSpace = -1,
  kErrorOpenFile = -2,
  kErrorEvent = -3,
  kErrorFileFull = -4
};

/// 事件类型
enum {
  kLogWFStart = -2,
  kLogStart = -1,
  kLogEnd = kLogStart,
  kLogWFEnd = kLogWFStart
};

/// 日志级别
enum {
  kLogNone = 0,
  kLogFatal = 0x01,     ///< fatal errors
  kLogWarning = 0x02,   ///< exceptional events
  kLogNotice = 0x04,    ///< informational notices
  kLogTrace = 0x08,     ///< program tracing
  kLogDebug = 0x10,     ///< full debugging
  kLogAll = 0xff        ///< everything
};

enum {
  kLogSelfBegin = 0x100,
  kLogSelfEnd = 0x107,
  kLogSelfMask = 0xff
};

/// FastLogHandle  log_sepc
enum {
  kLogTTY = 0x02,       ///< 日志在输出到日志文件的同时输出到标准出错(stderr)中
  kLogNewFile = 0x08,   ///< 创建新的日志文件,可以使每个线程都把日志打到不同文件中
  kLogSizeSplit = 0x10  ///< 按大小分割日志文件，不回滚
};

/// FastLogFile  flag
enum {
  kLogFileTruncate = 0x01,
  kLogFileFull = 0x02
};

struct FastLogCtrl {
  char processname[kProcNameLength];  ///< 包含进程名和参数 , 并用"_" 连接。 如: start_-d_/usr/mydir_c_myconf
  int32_t processid;
  char start_time[kTimeLenght];
  FastLogCtrl *next;
};

struct FastLogItem {
  char name[kNameLengthInData];
  int32_t value;
};

struct FastLogData {
  FastLogItem  item_str[kNameNumberInData];   ///< the process information
  char buff[kBufferSizeInData];               ///< record the WARNING and FATAL log
};

struct FastLogStat {
  int32_t events;     ///< 需要打的日志级别 0-15
  int32_t to_syslog;  ///< 输出到syslog 中的日志级别, 0-15
  int32_t spec;       ///< 扩展开关 0 or @ref kLogTTY or @ref kLogNewFile
};

struct FastLogFile {
  FILE *fp;                                   ///< 文件句柄
  int32_t flag;                               ///< 标志  @ref kLogFileTruncate | @ref kLogFileFull
  int32_t ref_cnt;                            ///< 引用计数
  int32_t max_size;                           ///< 文件可以记录的最大长度
  pthread_mutex_t file_lock;                  ///< 写文件锁
  char file_name[kMaxLogFileNameLength+1];    ///< 文件名字
  std::string last_date;
};

struct FastLogHandle {
  char used;                            ///< 0-未使用  1-已使用
  FastLogFile *pf;                      ///< log file
  FastLogFile *pf_wf;                   ///< log.wf file
  pthread_t tid;                        ///< 线程id
  int32_t mask;                         ///< 可以记录的事件的掩码
  int32_t log_syslog;                   ///< 输出到系统日志的事件掩码
  int32_t log_spec;                     ///< kLogTTY | kLogNewFile
  FastLogFile *spf[kMaxSelfDefineLog];  ///< 自定义日志文件句柄
};

/// 自定义日志的设置,可在OpenLog以及ThreadOpenLog中作为参数传入,设置kLogNewFile时,自定义日志与正常日志一样为
/// 线程文件,否则自定义日志为进程级日志
struct FastLogSelf {
  char name[kMaxSelfDefineLog][kMaxLogFileNameLength];  ///< 自定义日志文件名，系统自动在文件名后加后缀.sdf
  char flags[kMaxSelfDefineLog];                        ///< 决定当前自定义的日志是否输出,设置为1则生成自定义日志,0则不生成
  int32_t log_number;                                   ///< 自定义文件的数目,当设置为0时,不生成自定义文件
};


  /// @brief 打开日志文件（log和log.wf）并初始化日志对象(包括attach共享内存)
  /// @param [in] log_path : 日志文件所在目录
  /// @param [in] log_procname : 日志文件名前缀。如果文件名中包含'_'，则截断为'_'之前的字符串
  /// @param [in] maxlen : 单个日志文件的最大长度（unit: MB）
  /// @param [in|out] l_stat : 日志相关参数(用来设置log的特性)
  /// @param [in|out] self :  设置自定义log,具体使用方式请参见结构说明
  /// @return 0成功，-1失败
  /// @note 退出时需要调用CloseLog释放资源
  /// @see CloseLog ThreadOpenLog ThreadCloseLog
  int32_t OpenLog(const char *log_path, const char *log_procname, const int32_t maxlen, FastLogStat *l_stat, FastLogSelf *self = NULL);


  /// @brief 打印指定级别的字符串到对应的日志文件
  /// @param [in] event : 日志级别 (include kLogDebug,kLogTrace,kLogNotice,kLogWarning,kLogFatal)
  /// @param [in] fmt : 格式串
  /// @param [in] ... : 打印参数列表
  /// @return 0成功，<0失败
  /// @note 日志长度经过扩展，线程级日志可支持最大为2048字节，共享内存中的log信息在超过640字节依然会截断。
#ifdef  _UN_WRITELOG_FORMAT_CHECK
  int32_t WriteLog(const int32_t event, const char *fmt, ...);
#else
  int32_t WriteLog(const int32_t event, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#endif

  /// @brief 关闭日志文件并释放相关资源
  /// @param [in] iserr : 在日志中记录结束状态。0为正常，非0为不正常
  /// @return 0成功，-1失败
  int32_t CloseLog(const int32_t iserr);

  /// @brief 为本线程打开日志
  /// @param [in] threadname 用于打印的线程名字(如果设置了kLogNewFile,会根据threadname创建新的log文件)
  /// @param [in|out] l_stat 日志相关参数
  /// @param [in|out] self : 设置自定义log,具体使用方式请参见结构说明
  /// @return 0成功，-1失败
  /// @see ThreadCloseLog OpenLog CloseLog
  /// @note 当线程退出以后，要使用ThreadCloseLog来释放资源
  int32_t ThreadOpenLog(const char *threadname, FastLogStat *l_stat, FastLogSelf *self = NULL);

  /// @brief 关闭本线程的日志
  /// @param [in] iserr : 在日志中记录结束状态。0为正常，非0为不正常
  /// @return 0成功，-1失败
  int32_t ThreadCloseLog(const int32_t iserr);

  /// @brief 获得全局日志等级
  /// @return  int32_t
  int32_t GetLogLevel();
}       ///< end of namespcae FastLog
} } }   ///< end of namespace ganji::util::log
#endif  ///< _GANJI_UTIL_LOG_FAST_LOG_H_
