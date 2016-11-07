/**
 * @Copyright 2009 Ganji Inc.
 * @file    ganji/util/log/fast_log.cc
 * @namespace ganji::util::log
 * @version 1.0
 * @author  haohuang
 * @date    2010-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "fast_log.h"

#include <sys/stat.h>
#include <sys/param.h>
#include <string.h>
#include <syslog.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

namespace ganji { namespace util { namespace log { namespace FastLog {

static int32_t g_file_size;
static char g_log_path[kMaxLogFileNameLength+1] = "./";
static char g_proc_name[kMaxLogFileNameLength+1] = "";

static pthread_key_t g_log_fd = PTHREAD_KEYS_MAX;
static pthread_once_t g_log_unit_once = PTHREAD_ONCE_INIT;
static FastLogFile g_file_array[kLogFileNumber];
static FastLogFile g_file_stderr = {stderr, 0, 1, 0, PTHREAD_MUTEX_INITIALIZER, "/dev/stderr"};
static FastLogHandle  g_log_stderr = {1, &g_file_stderr, &g_file_stderr, 0, kLogAll, 0, 0, {NULL}};
static pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

static FastLogStat g_default_logstate = {kLogAll, kLogFatal, kLogTTY};


static int32_t namenum_inc = 0;
static FastLogItem  *item_str = NULL;
static int32_t item_loc = 0;
static FastLogCtrl *ctrl_ptr = NULL;
static FastLogData *data_ptr = NULL;
static FastLogCtrl *ctrl_str = NULL;
static FastLogData *data_str = NULL;

static int32_t   col_excur = -1;
static FastLogCtrl *ctrl_col_ptr = NULL;
static FastLogData *data_col_ptr = NULL;

static int32_t g_global_level = 16;     ///< 全局记录的日志等级


#define STRING_FATAL  "FATAL: "
#define STRING_WARNING  "WARNING: "
#define STRING_NOTICE "NOTICE: "
#define STRING_TRACE  "TRACE: "
#define STRING_DEBUG  "DEBUG: "
#define STRING_NULL "@NULL@"

static void log_fd_init() {
  pthread_key_create(&g_log_fd, NULL);
}

static int32_t ThreadLogSup() {
  pthread_once(&g_log_unit_once, log_fd_init);
  return 0;
}

static FastLogHandle *AllocateLogUnit() {
  FastLogHandle *log_fd;

  log_fd = reinterpret_cast<FastLogHandle *>(calloc(1, sizeof(FastLogHandle)));
  if (NULL == log_fd) {
    return NULL;
  }
  log_fd->tid = pthread_self();
  if (pthread_setspecific(g_log_fd, log_fd) != 0) {
    free(log_fd);
    return NULL;
  }
  return log_fd;
}

/// @brief 打开自定义日志
/// @param [in] self   自定义日志类型
/// @return  日志打开成功与否
/// @retval  -1 自定义日志打开失败，0自定义日志打开成功
static int32_t OpenLogSelf(const FastLogSelf *self) {
  if (NULL == self)
    return 0;

  if (self->log_number > kMaxSelfDefineLog || self->log_number < 0) {
    fprintf(stderr, "in fast_log.cc :self define log_number error!log_number = %d\n", self->log_number);
    fprintf(stderr, "in fast_log.cc:open error!\n");
    return -1;
  }
  for (int32_t i = 0; i < self->log_number; ++i) {
    if (strlen(self->name[i]) == 0 && self->flags[i] != 0) {
      fprintf(stderr, "in fast_log.cc :self define log error![%d]\n", i);
      fprintf(stderr, "in fast_log.cc:open error!\n");
      return -1;
    }
  }
  return 0;
}

static FILE *OpenLogFile(const char *name, char *mode) {
  size_t path_len;
  FILE   *fp;
  char   *path_end;
  char   path[kMaxLogFileNameLength+1];

  fp = fopen(name, mode);
  if (fp != NULL)
    return fp;

  path_end = strrchr((char *)name, '/');
  if (path_end != NULL) {
    path_len = (path_end>name+kMaxLogFileNameLength) ? kMaxLogFileNameLength : (size_t)(path_end-name);
  } else {
    path_len = strlen(name)>kMaxLogFileNameLength ? kMaxLogFileNameLength : strlen(name);
  }
  strncpy(path, name, path_len);
  path[path_len] = '\0';

  mkdir(path, 0700);

  return fopen(name, mode);
}

static FastLogFile *OpenFileUnit(const char *file_name, const int32_t flag, const int32_t max_size) {
  int32_t i;
  FastLogFile *file_free = NULL;

  pthread_mutex_lock(&file_lock);
  for (i = 0; i < kLogFileNumber; ++i) {
    if (NULL == g_file_array[i].fp) {
      if (NULL == file_free) {
        file_free = &g_file_array[i];
      }
      continue;
    }
    if (!strcmp(g_file_array[i].file_name, file_name)) {
      g_file_array[i].ref_cnt++;
      pthread_mutex_unlock(&file_lock);
      return &g_file_array[i];
    }
  }

  if (file_free != NULL) {
    file_free->fp = OpenLogFile(file_name, "a");
    if (NULL == file_free->fp) {
      pthread_mutex_unlock(&file_lock);
      return reinterpret_cast<FastLogFile *>(kErrorOpenFile);
    }
    pthread_mutex_init(&(file_free->file_lock), NULL);
    file_free->flag = flag;
    file_free->ref_cnt = 1;
    file_free->max_size = max_size;
    snprintf(file_free->file_name, kMaxLogFileNameLength, "%s", file_name);
  }
  pthread_mutex_unlock(&file_lock);
  return file_free;
}

void CloseFileUnit(FastLogFile *file_fd) {
  pthread_mutex_lock(&file_lock);
  file_fd->ref_cnt--;

  if (file_fd->ref_cnt <= 0) {
    if (file_fd->fp != NULL) {
      fclose(file_fd->fp);
    }
    memset(file_fd, 0, sizeof(FastLogFile));
  }
  pthread_mutex_unlock(&file_lock);
}

static FastLogHandle *GetLogUnit() {
  return reinterpret_cast<FastLogHandle *>(pthread_getspecific(g_log_fd));
}

static void FreeLogUnit() {
  FastLogHandle *log_fd;
  log_fd = GetLogUnit();
  if (log_fd != NULL) {
    pthread_setspecific(g_log_fd, NULL);
    free(log_fd);
  }
}

int32_t OpenLogEx(const char *file_name, const int32_t mask, const int32_t flag, const int32_t maxlen, FastLogHandle *log_fd,
                  FastLogSelf * self = NULL) {
  char tmp_name[kMaxLogFileNameLength];

  log_fd->mask = mask;
  g_log_stderr.mask = mask;
  snprintf(tmp_name, kMaxLogFileNameLength, "%slog", file_name);
  log_fd->pf = OpenFileUnit(tmp_name, flag, maxlen);
  if (NULL == log_fd->pf) {
    return kErrorNoSpace;
  } else if (reinterpret_cast<FastLogFile *>(kErrorOpenFile) == log_fd->pf) {
    return kErrorOpenFile;
  }

  snprintf(tmp_name, kMaxLogFileNameLength, "%slog.wf", file_name);
  log_fd->pf_wf = OpenFileUnit(tmp_name, flag, maxlen);
  if (NULL == log_fd->pf_wf) {
    CloseFileUnit(log_fd->pf);
    return kErrorNoSpace;
  } else if (reinterpret_cast<FastLogFile *>(kErrorOpenFile) == log_fd->pf_wf) {
    CloseFileUnit(log_fd->pf);
    return kErrorOpenFile;
  }
  if (self != NULL) {
    for (int32_t i = 0; i < self->log_number; ++i) {
      if (strlen(self->name[i]) != 0 && self->flags[i]) {
        snprintf(tmp_name, kMaxLogFileNameLength, "%s%s.sdf.log", file_name, self->name[i]);
        log_fd->spf[i] = OpenFileUnit(tmp_name, flag, maxlen);
        if (NULL == log_fd->spf[i] || reinterpret_cast<FastLogFile *>(kErrorOpenFile) == log_fd->spf[i]) {
          for (int32_t j = i - 1; j >= 0; --j) {
            if (self->flags[j]) {
              CloseFileUnit(log_fd->spf[j]);
            }
          }
          CloseFileUnit(log_fd->pf);
          CloseFileUnit(log_fd->pf_wf);
          return (NULL == log_fd->spf[i]) ? kErrorNoSpace : kErrorOpenFile;
        }
      }
    }
  }
  return 0;
}

static int32_t CheckLogFile(FastLogFile *file_fd, char *tmp_filename, size_t tmp_filename_size, int32_t split_file) {
  int32_t ret = 0, stat_ret;
  struct stat st;
  if (NULL == file_fd || NULL == file_fd->fp) {
    return -1;
  }
  if (0 == file_fd->max_size) {
    return 0;
  }
  if (stderr == file_fd->fp) {
    return -1;
  }

  stat_ret = stat(file_fd->file_name, &st);
  if (-1 == stat_ret) {
    fclose(file_fd->fp);
    file_fd->fp = OpenLogFile(file_fd->file_name, "a");
    if (NULL == file_fd->fp) {
      return -1;
    }
  }

  if ((file_fd->flag & kLogFileTruncate) && st.st_size + kMaxLogBuffSize >= file_fd->max_size) {
    if (!split_file) {
      // 先将旧日志 mv 成 .filename.tmp,
      // 在确认新的日志文件已经被建立后删除.这样可以防止意外造成文件丢失
      char * p = NULL;
      p = strrchr(file_fd->file_name, '/');
      if (NULL == p) {
        snprintf(tmp_filename, tmp_filename_size, ".%s.tmp", file_fd->file_name);
      } else {
        *p = '\0';
        snprintf(tmp_filename, tmp_filename_size, "%s/.%s.tmp", file_fd->file_name, p+1);
        *p = '/';
      }
    } else {
      int32_t suff_num = 0;
      time_t tt;
      struct tm vtm;
      time(&tt);
      localtime_r(&tt, &vtm);
      int32_t retl = snprintf(tmp_filename, tmp_filename_size, "%s.%04d%02d%02d%02d%02d%02d",
          file_fd->file_name, vtm.tm_year+1900, vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);

      struct stat statbuf;
      while (stat(tmp_filename, &statbuf) == 0) {
        suff_num++;
        snprintf(tmp_filename + retl , tmp_filename_size - retl, ".%d", suff_num);
      }
    }
    rename(file_fd->file_name, tmp_filename);
    fclose(file_fd->fp);
    file_fd->fp = OpenLogFile(file_fd->file_name, "a");
    if (NULL == file_fd->fp) {
      return -1;
    }
    ret = 1;
  } else {
    if (st.st_size >= file_fd->max_size) {
      file_fd->flag |= kLogFileFull;
    }
  }
  return ret;
}

/// @brief 将生成的buff输到实际的日志中
/// @param [in] file_fd   : 文件句柄
/// @param [in] buff   : 要写的buff
/// @return  是否成功
/// @retval  -1 失败， 0 成功
static int32_t RealWriteLogBuffer(FastLogFile * file_fd, const char *buff, const int32_t split_file) {
  int32_t check_flag = 0;
  char tmp_filename[kMaxLogFileNameLength];  ///< 临时文件名，日志文件回滚使用

  if (NULL == file_fd) {
    return -1;
  }

  pthread_mutex_lock(&(file_fd->file_lock));
  check_flag = CheckLogFile(file_fd, tmp_filename, sizeof(tmp_filename), split_file);

  if (check_flag >= 0) {
    fprintf(file_fd->fp, "%s\n", buff);
    fflush(file_fd->fp);
  }
  pthread_mutex_unlock(&(file_fd->file_lock));

  if (1 == check_flag && 0 == split_file) {
    remove(tmp_filename);
  }

  return 0;
}

static char *FormatTimeBuffer(char *t_ime, size_t t_ime_size) {
  time_t tt;
  struct tm vtm;

  time(&tt);
  localtime_r(&tt, &vtm);
  snprintf(t_ime, t_ime_size, "%02d-%02d %02d:%02d:%02d", vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
  return t_ime;
}


static int32_t RealWriteLogEx(const FastLogHandle *log_fd, const int32_t event, const char *fmt, va_list args) {
  size_t  bpos = 0;
  char buff[kMaxLogBuffSize];
  char now[kTimeLenght];
  FastLogFile *file_fd;
  buff[0] = '\0';
  file_fd = log_fd->pf;
  if (log_fd->mask < event) {
    return kErrorEvent;
  }
  switch (event) {
    case kLogStart:
      break;
    case kLogWFStart:
      file_fd = log_fd->pf_wf;
      break;
    case kLogNone:
      break;
    case kLogFatal:
      memcpy(&buff[bpos], STRING_FATAL, sizeof(STRING_FATAL));
      file_fd = log_fd->pf_wf;
      break;
    case kLogWarning:
      memcpy(&buff[bpos], STRING_WARNING, sizeof(STRING_WARNING));
      file_fd = log_fd->pf_wf;
      break;
    case kLogNotice:
      memcpy(&buff[bpos], STRING_NOTICE, sizeof(STRING_NOTICE));
      break;
    case kLogTrace:
      memcpy(&buff[bpos], STRING_TRACE, sizeof(STRING_TRACE));
      break;
    case kLogDebug:
      memcpy(&buff[bpos], STRING_DEBUG, sizeof(STRING_DEBUG));
      break;
    default:
      break;
  }

  if (file_fd->flag & kLogFileFull) {
    return kErrorFileFull;
  }

  FormatTimeBuffer(now, sizeof(now));
  bpos += strlen(&buff[bpos]);
  bpos += snprintf(&buff[bpos], kMaxLogBuffSize-bpos, "%s:  %s * %lu ", now, g_proc_name, log_fd->tid);
  vsnprintf(&buff[bpos], kMaxLogBuffSize-bpos, fmt, args);
  if (log_fd->log_syslog & event) {
    int priority;
    switch (event) {
      case kLogFatal:
        priority = LOG_ERR;
        break;
      case kLogWarning:
        priority = LOG_WARNING;
        break;
      case kLogNotice:
        priority = LOG_INFO;
        break;
      case kLogTrace:
      case kLogDebug:
        priority = LOG_DEBUG;
        break;
      default:
        priority = LOG_NOTICE;
        break;
    }
    syslog(priority, "%s\n", buff);
  }

  return RealWriteLogBuffer(file_fd, buff, (log_fd->log_spec & kLogSizeSplit));
}

static int32_t WriteLogEx(FastLogHandle *log_fd, int32_t event, const char *fmt, ...) {
  int32_t ret = 0;
  va_list args;

  va_start(args, fmt);
  ret = RealWriteLogEx(log_fd, event, fmt, args);
  va_end(args);

  return ret;
}

int32_t OpenLog(const char *log_path, const char *proc_name, const int32_t max_size, FastLogStat  *log_state, FastLogSelf *self) {
  char *end;
  FastLogHandle *log_fd;
  char file_name[kMaxLogFileNameLength+1];

  ThreadLogSup();

  if (OpenLogSelf(self) == -1) {
    return -1;
  }

  if (NULL == log_state) {
    log_state = &g_default_logstate;
  }

  if (max_size <= 0 || max_size >= kMaxLogFileSize) {
    g_file_size = (kMaxLogFileSize<<20);
  } else {
    g_file_size = (max_size<<20);
  }

  if (NULL == log_path || '\0' == log_path[0]) {
    g_log_path[0] = '.';
    g_log_path[1] = '\0';
  } else {
    strncpy(g_log_path, log_path, kMaxLogFileNameLength);
    g_log_path[kMaxLogFileNameLength] = '\0';
  }

  if (NULL == proc_name || '\0' == proc_name[0]) {
    snprintf(g_proc_name, sizeof(g_proc_name), "%s", "null");
  } else {
    strncpy(g_proc_name, proc_name, kMaxLogFileNameLength);
    g_proc_name[kMaxLogFileNameLength] = '\0';
    end = strchr(g_proc_name, '_');
    if (end != NULL) {
      *end = '\0';
    }
  }

  snprintf(file_name, kMaxLogFileNameLength, "%s/%s", g_log_path, g_proc_name);

  log_fd = AllocateLogUnit();
  if (NULL == log_fd) {
    fprintf(stderr, "in fast_log.cc :no space!\n");
    fprintf(stderr, "in fast_log.cc:open error!\n");
    return -1;
  }

  if (OpenLogEx(file_name, log_state->events, kLogFileTruncate, g_file_size, log_fd, self) != 0) {
    if (log_state->spec & kLogTTY) {
      fprintf(stderr, "in fast_log.cc:Can't open log file : %slog, exit!\n", proc_name);
    }
    FreeLogUnit();
    return -1;
  }

  g_global_level = log_state->events;
  log_fd->log_spec = log_state->spec;
  log_fd->log_syslog = log_state->to_syslog;

  if (log_state->spec & kLogTTY) {
    fprintf(stderr, "Open log file %slog success!\n", proc_name);
  }
  WriteLogEx(log_fd, kLogStart, "* Open process log by----%s\n=================================================", proc_name);
  WriteLogEx(log_fd, kLogWFStart, "* Open process log by----%s for wf\n========================================", proc_name);

  return 0;
}

int32_t  ThreadOpenLog(const char *thread_name, FastLogStat  *log_state, FastLogSelf * self) {
  pthread_t tid;
  FastLogHandle *log_fd = NULL;
  char file_name[kMaxLogFileNameLength];

  if (OpenLogSelf(self) == -1) {
    return -1;
  }
  tid = pthread_self();
  if (NULL == log_state) {
    log_state = &g_default_logstate;
  }
  if ((log_state->spec & kLogNewFile) && thread_name != NULL && thread_name[0] != '\0') {
    snprintf(file_name, kMaxLogFileNameLength, "%s/%s_%s_%lu_", g_log_path, g_proc_name, thread_name, tid);
  } else if (log_state->spec & kLogNewFile) {
    snprintf(file_name, kMaxLogFileNameLength, "%s/%s_null_%lu_", g_log_path, g_proc_name, tid);
  } else {
    snprintf(file_name, kMaxLogFileNameLength, "%s/%s", g_log_path, g_proc_name);
  }
  log_fd = AllocateLogUnit();
  if (NULL == log_fd) {
    fprintf(stderr, "in fast_log.cc :no space!\n");
    fprintf(stderr, "in fast_log.cc:open error!\n");
    return -1;
  }

  if (OpenLogEx(file_name, log_state->events, kLogFileTruncate, g_file_size, log_fd, self) != 0) {
    if (log_state->spec & kLogTTY) {
      fprintf(stderr, "in fast_log.cc:Can't open log file : %slog, exit!\n", thread_name);
    }
    FreeLogUnit();
    return -1;
  }

  log_fd->log_spec = log_state->spec;
  log_fd->log_syslog = log_state->to_syslog;

  if (log_state->spec & kLogTTY) {
    fprintf(stderr, "Open log file %slog success!\n", thread_name);
  }
  WriteLogEx(log_fd, kLogStart, "* Open thread log by----%s:%s\n===============================================", g_proc_name, thread_name);
  WriteLogEx(log_fd, kLogWFStart, "* Open thread log by----%s:%s for wf\n======================================", g_proc_name, thread_name);

  return 0;
}

static int32_t RealWriteLogExSelf(FastLogHandle *log_fd, int32_t event, const char *fmt, va_list args) {
  size_t  bpos = 0;
  char buff[kMaxLogBuffSize];
  char now[kTimeLenght];
  FastLogFile *file_fd;

  file_fd = log_fd->spf[event];

  if (file_fd->flag & kLogFileFull) {
    return kErrorFileFull;
  }

  FormatTimeBuffer(now, sizeof(now));
  bpos += snprintf(&buff[bpos], kMaxLogBuffSize-bpos, "SDF_LOG: LEVEL:%d %s: %s * %lu", event, now, g_proc_name, log_fd->tid);
  vsnprintf(&buff[bpos], kMaxLogBuffSize-bpos, fmt, args);
  return RealWriteLogBuffer(file_fd, buff, (log_fd->log_spec & kLogSizeSplit));
}


int32_t WriteLog(const int32_t event, const char* fmt, ...) {
  int32_t ret;
  va_list args;
  va_start(args, fmt);

  FastLogHandle *log_fd;

  log_fd = GetLogUnit();
  if (NULL == log_fd) {
    log_fd = &g_log_stderr;
  }

  int32_t self_log_id = event & kLogSelfMask;
  if (event >= kLogSelfBegin && event <= kLogSelfEnd && log_fd->spf[self_log_id] != NULL) {
    // 写自定义log
    ret = RealWriteLogExSelf(log_fd, self_log_id, fmt, args);
  } else if (log_fd->mask < event) {
    ret = kErrorEvent;
  } else {
    ret = RealWriteLogEx(log_fd, event, fmt, args);
  }
  va_end(args);

  if (log_fd->log_spec & kLogTTY) {
    va_start(args, fmt);
    ret = RealWriteLogEx(&g_log_stderr, event, fmt, args);
    va_end(args);
  }
  return ret;
}

static int32_t CloseLogFD(FastLogHandle *log_fd) {
  if (NULL == log_fd) {
    return -1;
  }

  if (log_fd->pf != NULL) {
    CloseFileUnit(log_fd->pf);
  }
  if (log_fd->pf_wf != NULL) {
    CloseFileUnit(log_fd->pf_wf);
  }
  for (int32_t i = 0; i < kMaxSelfDefineLog; ++i) {
    if (log_fd->spf[i] != NULL) {
      CloseFileUnit(log_fd->spf[i]);
    }
  }
  return 1;
}

static int32_t CloseLogEx(int32_t iserr, const char * close_info) {
  FastLogHandle  *log_fd;

  log_fd = GetLogUnit();

  if (NULL == log_fd) {
    return -1;
  }

  if (iserr) {
    WriteLogEx(log_fd, kLogEnd, "< ! > Abnormally end %s\n================================================", close_info);
    WriteLogEx(log_fd, kLogWFEnd, "< ! > Abnormally end %s\n================================================", close_info);
  } else {
    WriteLogEx(log_fd, kLogEnd, "< - > Normally end %s\n================================================", close_info);
    WriteLogEx(log_fd, kLogWFEnd, "< - > Normally end %s\n================================================", close_info);
  }

  CloseLogFD(log_fd);

  if (log_fd->log_spec & kLogTTY) {
    fprintf(stderr, "Close log successed.\n");
  }
  FreeLogUnit();
  return 0;
}

int32_t ThreadCloseLog(const int32_t iserr) {
  return CloseLogEx(iserr, "thread");
}

int32_t CloseLog(const int32_t iserr) {
  int32_t ret = CloseLogEx(iserr, "process");
  return ret;
}

int32_t GetLogLevel() {
  return  g_global_level;
}

void SetLogLevel(int32_t lv) {
  g_global_level = lv;
}

} } } }   ///< end of namespace ganji::util::log::FastLog
