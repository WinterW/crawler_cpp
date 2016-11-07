#ifndef _GANJI_UTIL_TOOL_MEMCACHE_H_
#define _GANJI_UTIL_TOOL_MEMCACHE_H_ 
#include <libmemcached/memcached.h>
#include <string>
#include <vector>
#include <map>

namespace ganji { namespace  util { namespace tool {
/**
 * memcache的封装,支付查找,批量查找,设置,删除, 线程不安全
*/
class Memcache
{
public:
	Memcache();
	~Memcache();
	/// 初始化, 每行是一个host:port的配置, port不填,则为11211; timeout为连接读写超时(毫秒), 定时调的话则会动态检测配置文件是否修改,如修改则重新初始化
	bool Init(const std::string& strConfFile, int timeout);
  
  /// 初始化, 每行是一个host:port的配置, port不填,则为11211
  bool Init(const std::vector<std::string>& host_vec, int timeout);
	
  /// 获取单个key的value
	std::string Get(const std::string& strKey);
	
  /// 获取多个key的值,结果保存在mapKey2Value中(value为空则没有值),返回值存在的个数
  /// try_fetch_times = -1 表示获取所有结果。try_fetch_times > 0 的值，表示尝试获取结果的次数，可能获取不完即退出。
	uint32_t Gets(std::map<std::string,std::string>* mapKey2Value, const int try_fetch_times = -1);
	
	/// 设置
	bool Set(const std::string& strKey, const std::string& strValue,time_t expiration_secs=7200, uint32_t flags=0);		
	
	/// expiration为0表示立即删除
	bool Delete(const std::string& strKey, int expiration = 0);

  char* LastError() {
    return error_;
  }

protected:
  ///
  void ErrorLog(const std::string &prompt, memcached_return rc);
  
private:
	memcached_st *m_client;    //长链接,多少个server,就最多有多少个socket链接,当前Memcache对象析构时,才会释放
  time_t conf_file_last_modify_time_;
  char error_[1024];
};
}}}
#endif
