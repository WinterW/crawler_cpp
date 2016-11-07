/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/sleep.h
 * @namespace     ganji::util::thread::Sleep
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * an implement of Sleep(milliseconds)
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_THREAD_SLEEP_H_
#define _GANJI_UTIL_THREAD_SLEEP_H_

#include "global.h"

namespace ganji { namespace util { namespace thread {

namespace Sleep {
  /// 睡眠指定的毫秒时间
  /// @param[in] milliseconds: milliseconds of sleep, if <=0 then immediate return.
  void DoSleep(int32_t milliseconds);
}
} } }  ///< end of namespace ganji::util::thread::Sleep
#endif  ///< _GANJI_UTIL_THREAD_SLEEP_H_
