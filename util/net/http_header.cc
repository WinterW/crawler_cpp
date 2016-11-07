/** 
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/util/net/http_header.cc
 * @namespace ganji::util::net
 * @version 1.0
 * @author jiafa
 * @date    2010-07-27
 * @brief create and resolve http head
 * Change Log:
 *
 */
#include "http_header.h"

#include <assert.h>
#include <iterator>

#include "http_opt.h"
#include "util/text/text.h"

using std::string;
using std::vector;
using std::map;

namespace Text = ganji::util::text::Text;
namespace Http = ganji::util::net::Http;

namespace ganji { namespace util { namespace net {

void HttpHeaderArray::SetHeader(const string &header, const string &value, bool merge) {
  HeaderEntryPtr entry_ptr;
  int index = LookupEntry(header, &entry_ptr);

  // If an empty value is passed in, then delete the header entry...
  // unless we are merging, in which case this function becomes a NOP.
  if (value.empty()) {
    if (!merge && index >= 0) {
      vector<HeaderEntryPtr>::iterator itr = headers_.begin();
      advance(itr, index);
      headers_.erase(itr);
    }
    return;
  }

  // Create a new entry, or...
  if (index < 0) {
    entry_ptr->header = header;
    entry_ptr->value = value;
    headers_.push_back(entry_ptr);
    return;
  }
  if (merge && CanAppendToHeader(header)) {
    // Append the new value to the existing value iff...
    if (header == HttpVar::kSet_Cookie || header == HttpVar::kWWW_Authenticate
        || header == HttpVar::kProxy_Authenticate) {
      // Special case these headers and use a newline delimiter to
      // delimit the values from one another as commas may appear
      // in the values of these headers contrary to what the spec says.
      entry_ptr->value += "\r\n";
    } else {
      // Delimit each value from the others using a comma (per HTTP spec)
      entry_ptr->value += (", ");
    }
    entry_ptr->value += value;
  } else {
    // Replace the existing string with the new value
    entry_ptr->value = value;
  }
}

int HttpHeaderArray::ClearHeader(const string &header) {
  HeaderEntryPtr entry_ptr;
  int index = LookupEntry(header, &entry_ptr);
  if (index >= 0) {
    vector<HeaderEntryPtr>::iterator itr = headers_.begin();
    advance(itr, index);
    headers_.erase(itr);
  }
  return index;
}
void HttpHeaderArray::ParseHttpQuery(const string &line, map <string, string> *presult) {
  vector<string> vec_segments;
//  presult->clear();

  //remove the content after the #
  size_t nSharpPos = line.find("#");
  string temp = line;
  if (nSharpPos != string::npos)
    temp = line.substr(0, nSharpPos);
  Text::Segment(temp, '&', &vec_segments);
  for (size_t i = 0; i < vec_segments.size(); ++i) {
    string line = vec_segments[i];
    if (line.empty())
      continue;
    size_t pos;
    pos = line.find('=');
    if (pos == string::npos)
      continue;
    string name  = line.substr(0, pos);
    string value = line.substr(pos + 1);
    Text::TrimAll("\t ", &name);
    Text::TrimAll("\t ", &value);
    (*presult)[name] = Http::DeescapeURL(value);
  }
}

bool HttpHeaderArray::GetHeader(const string &header, string *result) {
  assert(result != NULL);
  HeaderEntryPtr entry_ptr;
  if (LookupEntry(header, &entry_ptr) < 0) {
    return false;
  }
  *result = entry_ptr->value;
  return true;
}

int HttpHeaderArray::ParseHeader(const string &header) {
  vector<string> vec_segments;
  Text::Segment(header, '\n', &vec_segments);
  size_t i = 0;
  for ( ; i < vec_segments.size(); ++i) {
    string &line = vec_segments[i];
    Text::Trim("\r", &line);
    string hdr;
    string val;
    ParseHeaderLine(line, &hdr, &val);
  }
  return i;
}

void HttpHeaderArray::ParseHeaderLine(const string &line, string *hdr, string *val) {
  //
  // BNF from section 4.2 of RFC 2616:
  //
  //   message-header = field-name ":" [ field-value ]
  //   field-name     = token
  //   field-value    = *( field-content | LWS )
  //   field-content  = <the OCTETs making up the field-value
  //                     and consisting of either *TEXT or combinations
  //                     of token, separators, and quoted-string>
  //

  // We skip over mal-formed headers in the hope that we'll still be able to
  // do something useful with the response.

  string::size_type mhPos = line.find(":");
  if (mhPos == string::npos) {
    return;
  }
  *hdr = line.substr(0, mhPos);
  if (mhPos + 1 < line.length()) {
    *val = line.substr(mhPos);
  } else {
    *val = "";
  }
  SetHeader(*hdr, *val, true);
}

void HttpHeaderArray::Flatten(bool prune, string *buf) const {
  string::size_type count = Count();
  for (string::size_type i = 0; i < count; ++i) {
    HeaderEntryPtr entry_ptr =  headers_[i];
    // prune proxy headers if requested
    if (prune && ((entry_ptr->header == HttpVar::kProxy_Authorization) || (entry_ptr->header == HttpVar::kProxy_Connection)))
      continue;
    *buf += (entry_ptr->header);
    *buf += ": ";
    *buf += entry_ptr->value;
    *buf += "\r\n";
  }
}

void HttpHeaderArray::Clear() {
  headers_.clear();
}

//-----------------------------------------------------------------------------
// HttpHeaderArray <private>
//-----------------------------------------------------------------------------

int HttpHeaderArray::LookupEntry(const string &header, HeaderEntryPtr *entry_ptr) const {
  if (entry_ptr == NULL)
    return -1;
  size_t count = Count();
  for (size_t i = 0; i < count; ++i) {
    if (headers_[i]->header == header) {
      *entry_ptr =  headers_[i];
      return i;
    }
  }
  return -1;
}

bool HttpHeaderArray::CanAppendToHeader(const string &header) {
  return header != HttpVar::kContent_Type        &&
      header != HttpVar::kContent_Length      &&
      header != HttpVar::kUser_Agent          &&
      header != HttpVar::kReferer             &&
      header != HttpVar::kHost                &&
      header != HttpVar::kAuthorization       &&
      header != HttpVar::kProxy_Authorization &&
      header != HttpVar::kIf_Modified_Since   &&
      header != HttpVar::kIf_Unmodified_Since &&
      header != HttpVar::kFrom                &&
      header != HttpVar::kLocation            &&
      header != HttpVar::kMax_Forwards;
}
}}}
