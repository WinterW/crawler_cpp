/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/http_opt.h
 * @namespace     ganji::util::net
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 * http option!
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_NET_HTTP_OPT_H_
#define _GANJI_UTIL_NET_HTTP_OPT_H_

#include <string>

#include "global.h"


namespace ganji { namespace util { namespace net {
namespace Http {
  /// encoding the unsafe chars in the url
  /// @param [in[ url the url to be encoded
  /// @return the url after the encoding
  std::string EscapeURL(const std::string &url);

  /// convert the unsafe chars to their original format, the reverse process of EscapeURL
  /// @param [in] url the url to be processed
  /// @return the deescaped url
  std::string DeescapeURL(const std::string &url);

  /// top domain list
  const char kTopDomain[][10] = {
    ".com",
    ".edu",
    ".gov",
    ".int",
    ".mil",
    ".net",
    ".org",
    ".biz",
    ".info",
    ".pro",
    ".name",
    ".museum",
    ".coop",
    ".aero",
    ".xxx",
    ".idv"
  };

  /// parse url, to get protocol/domain/port
  /// @param[in] url the url to be processed
  /// @param[out] p_protocol protocol used
  /// @param[out] p_domain the domain of the url
  /// @param[out] p_port the requested port
  /// @return 0:success -1:failure
  int ParseUrl(const std::string &url,
      std::string *p_protocol,
      std::string *p_domain,
      int *p_port);

  /// get domain by url
  /// @param[in] url the url to be processed
  /// @param[out] domain the domain of the url
  /// @return 0:success -1:failure
  int GetUrlDomain(const std::string &url, std::string *domain);

  /// get main domain by url
  /// @param[in] url the url to be processed
  /// @param[out] main_domain the main domain of the url
  /// @return 0:success -1:failure
  int GetMainDomain(const std::string &url, std::string *main_domain);
}
} } }   ///< end of namespace ganji::util::net
#endif  ///< _GANJI_UTIL_NET_HTTP_OPT_H_
