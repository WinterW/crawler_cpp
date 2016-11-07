/**
 * @Copyright 2011 Ganji Inc.
 * @file    ganji/util/system/system.h
 * @namespace ganji::util::system
 * @version 1.0
 * @author  lisizhong
 * @date    2011-06-22
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_UTIL_SYSTEM_SYSTEM_H_
#define _GANJI_UTIL_SYSTEM_SYSTEM_H_

#include "global.h"

namespace ganji { namespace util { namespace system {
/// all functions here are static, so we use a namespace to warp these functions and make it looks like a class
namespace System {
  /// @brief get mem usage of current process in KB
  /// @param[out] p_mb memory usage in MB
  /// @return 0:success -1:failure
  int GetMemKb(int *p_kb);

  /// @brief get mem usage of current process
  /// @param[out] p_mb memory usage in MB
  /// @return 0:success -1:failure
  int GetMem(int *p_mb);
}
} } }   ///< end of namespace ganji::util::system::System
#endif  ///< _GANJI_UTIL_SYSTEM_SYSTEM_H_
