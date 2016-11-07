/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/socket_short_server.h
 * @namespace     ganji::util::net
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 * a socket server that fit short-connect
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_NET_SOCKET_SHORT_SERVER_H_
#define _GANJI_UTIL_NET_SOCKET_SHORT_SERVER_H_

#include <string>
#include <queue>

#include "global.h"
#include "util/thread/mutex.h"
#include "util/thread/condition.h"
#include "util/thread/thread.h"


namespace ganji { namespace util { namespace net {

class SocketShortServer {
 public:
	SocketShortServer();
	~SocketShortServer();

	bool Initialize(const uint16_t uport);
	bool Initialize(const std::string &addr_str,const uint16_t uport);
	
	bool Release();

	void RunListen();
	void RunSelect();

	virtual bool CheckMsg(const std::string &msg_str_) {
    return true;
  }

	void WaitMsg();
	bool GetMsg(int &socket, std::string &msg_str);

 private:
 	struct _ReceivedMsg {
		int socket_;
		int32_t status_;
		std::string msg_str_;
	};

 private:
 	std::string server_addr_;
	uint16_t server_port_;
	
	bool is_initialize_;
	bool is_release_;

	int server_socket_;

	ganji::util::thread::Thread *listen_thread_;
	ganji::util::thread::Thread *select_thread_;

	std::queue< int > socket_queue_;
	std::queue< _ReceivedMsg > request_queue_;
	ganji::util::thread::Condition request_cond_;
	
	ganji::util::thread::Mutex select_mutex_;
	ganji::util::thread::Mutex requeset_mutex_;
	
	bool stop_flag_;
};

class SocketWebServer : public SocketShortServer {
 public:
	bool CheckMsg(const std::string &msg_str) {
		if (msg_str.find("\r\n\r\n") != std::string::npos)
			return true;
		return false;
	}
};
} } }   ///< end of namespace ganji::util::net
#endif  ///< _GANJI_UTIL_NET_SOCKET_SHORT_SERVER_H_
