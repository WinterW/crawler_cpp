/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/ip_num.h
 * @namespace     ganji::util::net
 * @version       1.0
 * @author        lisizhong
 * @date          2011-07-12
 * 
 * http option!
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_NET_IP_NUM_H_
#define _GANJI_UTIL_NET_IP_NUM_H_

#include <string>

#include "global.h"

namespace ganji { namespace util { namespace net {
namespace IpNum {
  /// ip字符串转换为num
  /// @param [in] ip_str ip字符串
  /// @param [out] p_ip_num ip num
  /// @return 0:success -1:failure
  int Ip2Num(const std::string &ip_str, uint32_t *p_ip_num);

  /// ip num转换为ip字符串
  /// @param [in] ip_num ip num
  /// @param [out] p_ip_str ip字符串
  /// @return 0:success -1:failure
  int Num2Ip(uint32_t ip_num, std::string *p_ip_str);
}
} } }   ///< end of namespace ganji::util::net
#endif  ///< _GANJI_UTIL_NET_IP_NUM_H_
