/** 
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/util/net/httpvar.h
 * @namespace ganji::util::net
 * @version 1.0
 * @author  jiafa
 * @date    2010-07-27
 * define the http headers in this file
 * Change Log:
 *
 */
#ifndef _GANJI_UTIL_NET__HTTPVAR_H_
#define _GANJI_UTIL_NET__HTTPVAR_H_
#include <string>

namespace ganji { namespace util { namespace net {
/**
 * @namespace HttpVar
 *
 * @desc This class defines the http headers of the http protocals
 */
namespace HttpVar {
  const std::string kAccept = "Accept";
  const std::string kAccept_Charset = "Accept-Charset";
  const std::string kAccept_Encoding = "Accept-Encoding";
  const std::string kAccept_Language = "Accept-Language";
  const std::string kAccept_Ranges = "Accept-Ranges";
  const std::string kAge = "Age";
  const std::string kAllow = "Allow";
  const std::string kAuthentication = "Authentication";
  const std::string kAuthorization = "Authorization";
  const std::string kCache_Control = "Cache-Control";
  const std::string kConnection = "Connection";
  const std::string kContent_Base = "Content-Base";
  const std::string kContent_Encoding = "Content-Encoding";
  const std::string kContent_Language = "Content-Language";
  const std::string kContent_Length = "Content-Length";
  const std::string kContent_Location = "Content-Location";
  const std::string kContent_MD5 = "Content-MD5";
  const std::string kContent_Range = "Content-Range";
  const std::string kContent_Transfer_Encoding;  // "Content-Transfer-Encoding";
  const std::string kContent_Type = "Content-Type";
  const std::string kCookie = "Cookie";
  const std::string kDate = "Date";
  const std::string kDAV = "DAV";
  const std::string kDepth = "Depth";
  const std::string kDerived_From = "Derived-From";
  const std::string kDestination = "Destination";
  const std::string kETag = "Etag";
  const std::string kExpect = "Expect";
  const std::string kExpires = "Expires";
  const std::string kForwarded = "Forwarded";
  const std::string kFrom = "From";
  const std::string kHost = "Host";
  const std::string kIf = "If";
  const std::string kIf_Match = "If-Match";
  const std::string kIf_Match_Any = "If-Match-Any";
  const std::string kIf_Modified_Since = "If-Modified-Since";
  const std::string kIf_None_Match = "If-None-Match";
  const std::string kIf_None_Match_Any = "If-None-Match-Any";
  const std::string kIf_Range = "If-Range";
  const std::string kIf_Unmodified_Since = "If-Unmodified-Since";
  const std::string kKeep_Alive = "Keep-Alive";
  const std::string kLast_Modified = "Last-Modified";
  const std::string kLock_Token = "Lock-Token";
  const std::string kLink = "Link";
  const std::string kLocation = "Location";
  const std::string kMax_Forwards = "Max-Forwards";
  const std::string kMessage_Id = "Message-Id";
  const std::string kMime = "Mime";
  const std::string kOverwrite = "Overwrite";
  const std::string kPragma = "Pragma";
  const std::string kProxy_Authenticate = "Proxy-Authenticate";
  const std::string kProxy_Authorization = "Proxy-Authorization";
  const std::string kProxy_Connection = "Proxy-Connection";
  const std::string kRange = "Range";
  const std::string kReferer = "Referer";
  const std::string kRetry_After = "Retry-After";
  const std::string kServer = "Server";
  const std::string kSet_Cookie = "Set-Cookie";
  const std::string kSet_Cookie2 = "Set-Cookie2";
  const std::string kStatus_URI = "Status-URI";
  const std::string kTE = "TE";
  const std::string kTitle = "Title";
  const std::string kTimeout = "Timeout";
  const std::string kTrailer = "Trailer";
  const std::string kTransfer_Encoding = "Transfer-Encoding";
  const std::string kURI = "URI";
  const std::string kUpgrade = "Upgrade";
  const std::string kUser_Agent = "User-Agent";
  const std::string kVary = "Vary";
  const std::string kVersion = "Version";
  const std::string kWWW_Authenticate = "WWW-Authenticate";
  const std::string kWarning = "Warning";
  const std::string kConnect = "CONNECT";
  const std::string kCopy = "COPY";
  const std::string kDelete = "DELETE";
  const std::string kGet = "GET";
  const std::string kHead = "HEAD";
  const std::string kIndex = "INDEX";
  const std::string kLock = "LOCK";
  const std::string kM_Post = "M-POST";
  const std::string kMkcol = "MKCOL";
  const std::string kMove = "MOVE";
  const std::string kOptions = "OPTIONS";
  const std::string kPost = "POST";
  const std::string kPropfind = "PROPFIND";
  const std::string kProppatch = "PROPPATCH";
  const std::string kPut = "PUT";
  const std::string kTrace = "TRACE";
  const std::string kUnlock = "UNLOCK";
};
} } }
#endif  /// _GANJI_UTIL_NET__HTTPVAR_H_
