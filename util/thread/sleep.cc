/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/thread/sleep.cc
 * @namespace     ganji::util::thread::Sleep
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-20
 * 
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "sleep.h"
#include <sys/select.h>

namespace ganji { namespace util { namespace thread { namespace Sleep {

void DoSleep(int32_t milliseconds) {
  if (milliseconds <= 0) {
    return;
  }
  struct timeval tv;
  tv.tv_sec = milliseconds / 1000;
  tv.tv_usec = (milliseconds % 1000) * 1000;
  select(0, 0, 0, 0, &tv);
};

} } } }
