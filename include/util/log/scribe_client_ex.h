/*
Author: pengcheng
Date: 2011-05-11
Description: Scribe客户端，提供重连、消息缓存、重发机制
*/

#ifndef _GANJI_UTIL_LOG_SCRIBE_CLIENT_EX_H
#define _GANJI_UTIL_LOG_SCRIBE_CLIENT_EX_H

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <string>
#include <vector>
#include "ganji/util/log/scribe.h"

using namespace std;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::protocol;
using namespace scribe::thrift;

class ScribeClientEx {
 public:
  ScribeClientEx(const string &host, int port, const string &cache_folder);
  bool Init();
  bool Send(const vector<LogEntry> &logs);

 protected:
  void Clear();
  bool Reconnect();
  void FindCacheFiles(vector<string> &cache_files);

 private:
  string host_;
  int port_;
  string cache_folder_;
  TSocket *sock_;
  TFramedTransport *trans_;
  TBinaryProtocol *prot_;
  scribeClient *client_;
};

#endif

