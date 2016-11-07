/**
* @Copyright (c) 2010 Ganji Inc.
* @file          ganji/util/log/scribe_log.h
* @namespace     ganji::util::log
* @version       1.0
* @author        miaoxijun
* @date          2010-09-26
*
* ThriftLog
* 使用如下
*
* main thread                                            child thread                     child thread
*
*      ThriftLog::thrift_log_init
*                                                        ThriftLog::thrift_log_write      ThriftLog::thrift_log_write
*      ThriftLog::thrift_log_uninit
*/
#ifndef _GANJI_UTIL_LOG_THRIFT_LOG_H_
#define _GANJI_UTIL_LOG_THRIFT_LOG_H_
#include <stdlib.h>

namespace ganji { namespace util { namespace log {
namespace ThriftLog {

/// ThriftLog_Type
enum ThriftLog_Type {
    FATAL = 0x01,
    WARNING = 0x02,
    NOTICE = 0x04,
    TRACE = 0x08,
    DEBUG = 0x10
};

/// @brief 初始化日志系统，使用日志系统前必须先调用该函数
/// param [in] hostip : 服务器IP
/// param [in] hostport : 服务器打开端
/// param [in] maxcache 日志最大缓存数
/// example : LogInit("192.168.1.61",11463,2000);
extern int32_t LogInit(const char * hostip, int32_t hostport, int32_t maxcache = 999999);



/// @brief日志记录函数,推荐内部使用该函数,单条message支持最大1024字节
/// param [in] category : 日志类别
/// param [in] message : 日志内容
/// example : LogWrite(WARNING,"uc",""connect failed");


extern int32_t LogWrite(int32_t log_type, const char * category, const char * message);


/// @brief日志记录函数,单条message支持最大1024字节
/// param [in] category : 日志类别
/// param [in] format : 格式串
/// example : LogWrite("uc","%s %s %d","debug","connect failed",500);

extern int32_t LogWrite(const char * category, const char * format, ...);


/// @brief 清理日志系统，使用完日志系统必须调用该函数

extern int32_t LogUninit();

}     ///< end of namespace ThriftLog
} } }  ///< end of namespace ganji::util::log
#endif  ///< _GANJI_UTIL_LOG_THRIFT_LOG_H_
