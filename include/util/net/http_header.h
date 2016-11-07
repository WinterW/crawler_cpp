/** 
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/util/net/http_header.h
 * @namespace ganji::util::net
 * @version 1.0
 * @author jiafa
 * @date    2010-07-27
 * @brief create and resolve http head
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_NET_HTTPHEADERARRAY_H_
#define _GANJI_UTIL_NET_HTTPHEADERARRAY_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <map>

#include "http_var.h"

namespace ganji { namespace util { namespace net {

/*
 * @struct HeaderEntry
 *
 * @desc an entry of the http header
 */
struct HeaderEntry {
    std::string header;  ///< the header name or key
    std::string value;   ///< the header entry value
};
typedef boost::shared_ptr<HeaderEntry> HeaderEntryPtr;

/*
 * @class HttpHeaderArray
 *
 * @desc the http header array, can be used to set and get http head
 */
class HttpHeaderArray {
 public:
   HttpHeaderArray() {}

   ~HttpHeaderArray() {
     Clear();
   }

   /// set header with some value
   /// @param [in] header the header key
   /// @param [in] value the header value
   /// @param [in] merge merge the value or replace
   void SetHeader(const std::string &header, const std::string &value, bool merge);

   /// get the header value with some key
   /// @param [in] header the header key
   /// @param [out] value the header value
   /// @return get a valid header value or not found
   bool GetHeader(const std::string &header, std::string *value);

   /// clean header entry
   /// @param [in] header the header key
   /// @return if >=0, succeed, ow. on not found header
   int ClearHeader(const std::string &header);

   /// parse one line of the header
   /// @param [in] line one line of content
   /// @param [out] hdr the header key
   /// @param [out] val the haeder value
   void ParseHeaderLine(const std::string &line, std::string *hdr, std::string *val);

   /// parse one line of the header
   /// @param [in] line one line of content
   /// @param [out] presult Parse result
   static void ParseHttpQuery(const std::string &line, std::map <std::string, std::string> *presult);

   /// parse the header
   /// @param [in] header the string format of the header
   /// @return the number of header entries in the header
   int ParseHeader(const std::string &header);

   /// convert the array to the string format
   /// @param [in] prune_proxy_headers whether remove the proxy header
   /// @param [out] the string format of the header
   void Flatten(bool prune_proxy_headers, std::string *flat) const;

   /// return the number of the header entries in the instance
   /// @return the number
   size_t Count() const {
     return headers_.size();
   }

   /// clear all the header entries in the instance
   void Clear();

 private:
   /// search a header entry in the array
   /// @param [in] header the header key
   /// @param [out] entry_ptr the entry pionter to be returned
   /// @return -1 no found, ow, the index of the entry in the list
   int LookupEntry(const std::string &header, HeaderEntryPtr *entry_ptr) const;

   /// determine whether the header can have more contents
   /// @param [in] header the header key
   /// @return if true, can append, ow can't
   static bool CanAppendToHeader(const std::string &header);
   std::vector<HeaderEntryPtr> headers_;  ///< the headers have added
};
}}}
#endif  /// _GANJI_UTIL_NET_HTTPHEADERARRAY_H_
