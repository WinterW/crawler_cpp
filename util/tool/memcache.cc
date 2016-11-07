#include "memcache.h"

#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <iostream>
#include <fstream>
using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

#include "util/log/thread_fast_log.h"
using namespace ganji::util::log::ThreadFastLog;

namespace ganji { namespace util {namespace tool {
Memcache::Memcache()
{
  m_client=NULL;
  conf_file_last_modify_time_ = 0;
}
Memcache::~Memcache()
{
  if(m_client)
    memcached_free(m_client);
}
bool Memcache::Init(const string& strConfFile, int timeout)
{
  struct stat st;
  if(stat(strConfFile.c_str(),&st)!=0){
    cerr<<"memcache file not found:" << strConfFile << endl;
    return false;
  }
  if(st.st_mtime == conf_file_last_modify_time_){
    return true;
  }
  conf_file_last_modify_time_ = st.st_mtime;

  ifstream fin(strConfFile.c_str());
  if(!fin){
    cerr<<"open memcache conf file fail:" << strConfFile <<endl;
    return false;
  }
  
  vector<string> host_vec;
  string line;
  while(getline(fin, line)){
    host_vec.push_back(line);
  }
  return Init(host_vec, timeout);
}

bool Memcache::Init(const vector<string>& host_vec, int timeout) {
  if (host_vec.empty()) {
    return false;
  }

  string host;
  uint32_t port;
 
  memcached_return rc;
  memcached_st* client = memcached_create(NULL);
  for(size_t i = 0; i < host_vec.size(); i++) {
    string line = host_vec[i];
    if(line[0] == '#') {
      continue;
    }
    
    size_t pos = line.find("\r");
    if (pos != string::npos) {
      pos = line.find("\n");
    }
    if (pos != string::npos) {
      line = line.substr(0, pos);
    }
     
    pos = line.find(":");
    if(pos != string::npos) {
      host = line.substr(0, pos);
      port = atoi(line.substr(pos+1).c_str());
    }
    else{
      host = line;
      port = 11211;
    }

    rc = memcached_server_add(client, host.c_str(), port);
    if (rc != MEMCACHED_SUCCESS) {
      ErrorLog("memcached_server_add error\t" + line, rc);
      return false;
    }
  }
  
  if(m_client) {
    memcached_free(m_client);
  }
  m_client = client;
  rc = memcached_behavior_set(m_client, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT); //重要,必须调用此语句使"一致性hash生效"
  if (rc != MEMCACHED_SUCCESS) {
    ErrorLog("memcached_behavior_set MEMCACHED_BEHAVIOR_DISTRIBUTION error", rc);
    return false;
  }
  
  rc = memcached_behavior_set(m_client, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, timeout); //单位,毫秒,最终用于setsockopt, SO_RCVTIMEO, 不设置会永远阻塞
  if (rc != MEMCACHED_SUCCESS) {
    ErrorLog("memcached_behavior_set MEMCACHED_BEHAVIOR_RCV_TIMEOUT error", rc);
    return false;
  }
  
  rc = memcached_behavior_set(m_client, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, timeout); //单位,毫秒,最终用于poll的最后一个参数,不设置会默认为1000
  if (rc != MEMCACHED_SUCCESS) {
    ErrorLog("memcached_behavior_set MEMCACHED_BEHAVIOR_CONNECT_TIMEOUTerror", rc);
    return false;
  }
  return true;
}

bool Memcache::Set(const string& strKey, const string& strValue, time_t expiration_secs, uint32_t flags)
{
  memcached_return rc = memcached_set(m_client, strKey.c_str(), strKey.size(), strValue.c_str(), strValue.size(),  expiration_secs, flags);
  if (rc != MEMCACHED_SUCCESS) {
    ErrorLog("memcached_set error\t" + strKey, rc);
    return false;
  }
  return true;
}

bool Memcache::Delete(const string& strKey, int expiration)
{
  memcached_return rc = memcached_delete(m_client, strKey.c_str(), strKey.size(), expiration); //最后一个参数表示多少秒后再删除,0表示立即删除
  if (rc != MEMCACHED_SUCCESS) {
    ErrorLog("memcached_delete error\t" + strKey, rc);
    return false;
  }
  return true;
}

string Memcache::Get(const string& strKey)
{
  string value;
  size_t len=0;
  memcached_return rc;
  uint32_t flag;
  char* data = memcached_get(m_client, strKey.c_str(), strKey.size(), &len, &flag, &rc);
  if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOTFOUND){
    ErrorLog("memcached_get error\t" + strKey, rc);
    if(NULL != data) {
      free(data);
      data = NULL;
    }
    return value;
  }
  if(data != NULL) {
    value = string(data, len);
    free(data);
  }
  return value;    
}

uint32_t Memcache::Gets(map<string,string>* pmapKey2Value, const int try_fetch_times)
{
  assert(pmapKey2Value != NULL);
  if (pmapKey2Value->size() == 0)
    return 0;
  map<string, string>& mapKey2Value = *pmapKey2Value;
  uint32_t nExsitCount=0;
  uint32_t num_keys=mapKey2Value.size();
  const char **keys = new const char*[num_keys];
  size_t* keys_length=new size_t[num_keys];
  size_t i=0;
  for(map<string,string>::iterator itMap=mapKey2Value.begin(); itMap!=mapKey2Value.end(); ++itMap){
    const string& strKey=itMap->first;
    keys[i] = (char *)strKey.c_str();
    keys_length[i] = strKey.size();
    i++;
  }

  memcached_return rc = memcached_mget(m_client, keys, keys_length, num_keys); 
  if (rc != MEMCACHED_SUCCESS) {
    ErrorLog("memcached_mget error\t" + string(keys[0]), rc);
  } else {
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    uint32_t flag;
    int cur_fetch_time = 0;
    while (true) {
      return_value = memcached_fetch(m_client, return_key, &return_key_length, &return_value_length, &flag, &rc);
      // 有key匹配，在全部结果返回后，rc = MEMCACHED_END； 没有key匹配，rc = MEMCACHED_NOTFOUND；
      if(rc == MEMCACHED_END || rc == MEMCACHED_NOTFOUND || rc == MEMCACHED_NOT_SUPPORTED){
        break;
      }
      if(return_value){
        string strKey = string(return_key,return_key_length);
        mapKey2Value[strKey] = string(return_value,return_value_length);
        free(return_value);
        return_value = NULL;
        nExsitCount++;
      }
      if (try_fetch_times > 0) {
        if (++cur_fetch_time >= try_fetch_times) {
          break;
        }
      }
    }
    if(NULL != return_value) {
       free(return_value);
       return_value = NULL;
    }
  }

  delete[] keys;
  delete[] keys_length;
  return nExsitCount;
}

void Memcache::ErrorLog(const string &prompt, memcached_return rc){
    snprintf(error_, 1024, "%s\trc=%d\terrno=%d\terror=%s\n",  prompt.c_str(), rc, errno, 
        rc == MEMCACHED_ERRNO ? strerror(errno): memcached_strerror(m_client, rc));
}

}}}
