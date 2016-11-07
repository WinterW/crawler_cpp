#include <unistd.h>
#include <sys/types.h>
#include <sys/dir.h>
#include "scribe_client_ex.h"
#include "ganji/util/file/file.h"
#include "ganji/util/text/text.h"
#include <Thrift.h>
#include <boost/shared_ptr.hpp>

using namespace ganji::util::file::File;
using namespace ganji::util::text::Text;
using namespace ::apache::thrift;
extern int alphasort();

ScribeClientEx::ScribeClientEx(const string &host, int port, const string &cache_folder) {
  host_ = host;
  port_ = port;
  cache_folder_ = cache_folder;
  sock_ = NULL;
  trans_ = NULL;
  prot_ = NULL;
  client_ = NULL;
}

void ScribeClientEx::Clear() {
  if (client_ != NULL) {
    delete client_;
    client_ = NULL;
  }
  ///< 删除client的时候，智能指针被自动销毁，不需要delete
  prot_ = NULL;
  trans_ = NULL;
  sock_ = NULL;
}

bool ScribeClientEx::Reconnect() {
  try {
    sock_ = new TSocket(host_.c_str(), port_);
    sock_->open();
    boost::shared_ptr<TSocket> sock_ptr(sock_);

    trans_ = new TFramedTransport(sock_ptr);
    boost::shared_ptr<TFramedTransport> trans_ptr(trans_);

    prot_ = new TBinaryProtocol(trans_ptr);
    boost::shared_ptr<TBinaryProtocol> prot_ptr(prot_);

    client_ = new scribeClient(prot_ptr);
    return true;
  }
  catch (const TException &ex) {
    Clear();
    return false;
  }
  catch (...) {
    Clear();
    return false;
  }
}

bool ScribeClientEx::Init() {
  ///< 尝试写一个空文件，看文件夹是否可写
  string try_file = cache_folder_ + "/" + "scribeclientextryfile";
  FILE *fp = fopen(try_file.c_str(), "w");
  if (fp == NULL) {
    return false;
  }
  fclose(fp);

  ///< 连接服务器
  if (! Reconnect()) {
    return false;
  }
  return true;
}

void ScribeClientEx::FindCacheFiles(vector<string> &cache_files) {
  cache_files.clear();

  struct dirent **namelist;
  int n = scandir(cache_folder_.c_str(), &namelist, 0, alphasort);
  if (n < 0) {
    return;
  }
  else {
    while (n--) {
      string str_name = namelist[n]->d_name;
      if (str_name.find("scribeclient.cache") == str_name.npos) {
        free(namelist[n]);
        continue;
      }
      cache_files.push_back(cache_folder_ + "/" + str_name);
      free(namelist[n]);
    }
    free(namelist);
  }  
}

bool ScribeClientEx::Send(const vector<LogEntry> &logs) {
  if (client_ == NULL) {
    Reconnect();
  }
  if (client_ != NULL) {
    ///< 如果处于可连接状态，则将缓存的数据先发送出去
    vector<string> cache_files;
    FindCacheFiles(cache_files);
    for (size_t ui = 0; ui < cache_files.size(); ++ui) {
      vector<string> lines;
      if (! LoadListFromFile(cache_files[ui], 0, &lines)) {
        break;
      }
      vector<LogEntry> log_entries;
      log_entries.reserve(lines.size());
      for (size_t uj = 0; uj < lines.size(); ++uj) {
        vector<string> segs;
        Segment(lines[uj], '\t', &segs);
        if (segs.size() != 2) {
          continue;
        }
        LogEntry le;
        le.category = segs[0];
        le.message = segs[1];
        log_entries.push_back(le);
      }
      try {
        if (ResultCode::TRY_LATER == client_->Log(log_entries)) {
          Clear();
          break;
        }
      }
      catch (const TException &except) {
        Clear();
      }
      ///< 发送完一个缓存文件后，清除这个文件
      remove(cache_files[ui].c_str());
    }
  }

  bool need_cache = false;
  if (client_ != NULL) {
    ///< 如果仍然处于可连接状态，则将本次数据发送出去
    try {
      if (ResultCode::TRY_LATER == client_->Log(logs)) {
        need_cache = true;
        Clear();
      }
    }
    catch (const TException &except) {
      Clear();
    }
  }
  if (need_cache || client_ == NULL) {
    ///< 缓存本次数据
    char buf[256];
    sprintf(buf, "%s/scribeclient.cache-%d-%d", cache_folder_.c_str(), time(NULL), rand());
    FILE *fp = fopen(buf, "w");
    for (size_t ui = 0; ui < logs.size(); ++ui) {
      fprintf(fp, "%s\t%s\n", logs[ui].category.c_str(), logs[ui].message.c_str());
    }
    fclose(fp);
  }
  return client_ != NULL;
}
