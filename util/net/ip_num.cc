/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/ip_num.cc
 * @namespace     ganji::util::net
 * @version       1.0
 * @author        lisizhong
 * @date          2010-07-12
 * 
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "ip_num.h"

#include <arpa/inet.h>
#include <string>

using std::string;

namespace ganji { namespace util { namespace net { namespace IpNum {
int Ip2Num(const string &ip_str, uint32_t *p_ip_num) {
  struct in_addr n_ip_num;
  if (inet_aton(ip_str.c_str(), &n_ip_num) == 1) {
    *p_ip_num = n_ip_num.s_addr;
    return 0;
  }
  return -1;
}

int Num2Ip(uint32_t ip_num, string *p_ip_str) {
  char buf[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &ip_num, buf, INET_ADDRSTRLEN) != NULL) {
    *p_ip_str = buf;
    return 0;
  }
  return -1;
}
} } } }   ///< end of namespace ganji::util::net::IpNum
