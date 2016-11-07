/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/socket_short_server.cc
 * @namespace     ganji::util::net
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-25
 * 
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#include "socket_short_server.h"

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <list>
#include <vector>


using std::string;
using std::queue;
using std::list;
using std::vector;
using ganji::util::thread::Mutex;
using ganji::util::thread::MutexGuard;
using ganji::util::thread::Condition;
using ganji::util::thread::Thread;

namespace ganji { namespace util { namespace net {

void *_RunListen(void * param) {
	if (!param)
		return 0;
	((SocketShortServer *) param)->RunListen();
}

void *_RunSelect(void * param) {
	if (!param)
		return 0;
	((SocketShortServer *) param)->RunSelect();
}

SocketShortServer::SocketShortServer() {
	is_initialize_ = false;
	is_release_ = false;
	stop_flag_ = true;
}

SocketShortServer::~SocketShortServer() {
	if (is_initialize_ && (!is_release_)) {
		Release();
  }
}


bool SocketShortServer::Initialize(const string &straddr, const uint16_t uport) {	
	if (is_initialize_)
		return true;

	server_addr_ = straddr;
	server_port_ = uport;

	server_socket_ = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == server_socket_) {
		return false;
	}

	int flag = 1;
	setsockopt (server_socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
	
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	if (server_addr_.empty())
		sin.sin_addr.s_addr=htonl(INADDR_ANY);
	else
		sin.sin_addr.s_addr = inet_addr(server_addr_.c_str());
	memset(sin.sin_zero,0,8);
	sin.sin_port=htons(server_port_);

	if (bind(server_socket_, (struct sockaddr*)&sin, sizeof(sin)) != 0) {
		close(server_socket_);
		server_socket_ = -1;
		return false;
	}

	if (listen(server_socket_,1000) != 0)	{
		close(server_socket_);
		server_socket_ = -1;
		return false;
	}
	
	stop_flag_ = false;

	listen_thread_ = Thread::CreateThread(_RunListen, this);
	select_thread_ = Thread::CreateThread(_RunSelect, this);
	
	//listen_thread_->DetachThread();
	listen_thread_->ResumeThread();

	//select_thread_->DetachThread();
	select_thread_->ResumeThread();
	
	is_initialize_ = true;
	return true;
}

bool SocketShortServer::Initialize(const uint16_t uport) {
	return Initialize("",uport);
}

bool SocketShortServer::Release() {
	if (is_release_)
		return true;

	stop_flag_ = true;
	
	request_cond_.Signal();
	
	if (server_socket_)
		close(server_socket_);
	server_socket_ = -1;
	void *return_value;

	if (0 != listen_thread_) {
		listen_thread_->WaitThread(&return_value);
		listen_thread_ = 0;
	}

	if (0 != select_thread_) {
		select_thread_->WaitThread(&return_value);
		select_thread_ = 0;
	}

	is_release_ = true;
	return true;
}

void SocketShortServer::RunListen() {
	struct timeval tv;
	fd_set fdset;
	
	int dida = 0;

	while (!stop_flag_) {
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		FD_ZERO(& fdset);
		FD_SET(server_socket_, & fdset);
		if (-1 != select(server_socket_ + 1, & fdset, NULL, NULL, &tv)) {
			if( stop_flag_ )
				break;
			if (! FD_ISSET(server_socket_, & fdset))
				continue;

			struct sockaddr sockAddr;
			memset(&sockAddr, 0, sizeof(struct sockaddr));
			socklen_t sockLen = sizeof(struct sockaddr);
			int acceptedSock = accept(server_socket_, & sockAddr, & sockLen);
			if (acceptedSock < 0)
				continue;
			select_mutex_.Lock();
			socket_queue_.push(acceptedSock);
			select_mutex_.Unlock();
		}
	}
}

void SocketShortServer::RunSelect() {
	list<_ReceivedMsg> listSock;
	_ReceivedMsg rmsg;
	vector<_ReceivedMsg> vecRMsg;
	vecRMsg.reserve(10);

	while (!stop_flag_) {
		char * pchBuf = 0;
		int iMaxSocket = 0;
		
		vecRMsg.resize(0);

		select_mutex_.Lock();
		while (!socket_queue_.empty()) {
			int acceptedSock = socket_queue_.front();
			socket_queue_.pop();
			rmsg.socket_ = acceptedSock;
			rmsg.status_ = 1;
			rmsg.msg_str_.clear();
			listSock.push_back(rmsg);
		}
		select_mutex_.Unlock();

		fd_set fdset;
		FD_ZERO(& fdset);
		list<_ReceivedMsg>::iterator itSock = listSock.begin();
		uint32_t fdsize = 0;
		while (itSock != listSock.end()) {	
			if( fdsize++ > 1020)
				break;
			FD_SET((*itSock).socket_, & fdset);
			if (iMaxSocket < (*itSock).socket_) {
				iMaxSocket = (*itSock).socket_;
			}
			++itSock;
		}

		struct timeval tvSpec;
		tvSpec.tv_sec = 0;
		tvSpec.tv_usec = 1000;
		int iSelectRet = select(iMaxSocket + 1, & fdset, NULL, NULL, & tvSpec);
		if(stop_flag_)
			break;
		if (iSelectRet > 0) {
			pchBuf = new char[0x2000]; //最大长度12.8k
			itSock = listSock.begin();
			while (itSock != listSock.end()) {
				if (!FD_ISSET( (*itSock).socket_, & fdset)) {
					itSock++;
					continue;
				}

				int iRecvLen = recv( (*itSock).socket_, pchBuf, 0x2000, 0);
				if (iRecvLen <= 0) {
					close( (*itSock).socket_ );
					itSock = listSock.erase(itSock);
					continue;
				}

				pchBuf[iRecvLen] = 0;

				_ReceivedMsg & recvMsg = (*itSock);
				recvMsg.status_++;
				recvMsg.msg_str_ += string(pchBuf);
				if (CheckMsg( recvMsg.msg_str_)) {
					vecRMsg.push_back(recvMsg);
					itSock = listSock.erase(itSock);
				} else {
					if (recvMsg.status_ > 3) {
						close( (*itSock).socket_ );
						itSock = listSock.erase(itSock);
					} else {
						itSock++;
					}
				}
			}
			
			requeset_mutex_.Lock();
			for(int i=0; i<vecRMsg.size(); i++) {
				request_queue_.push(vecRMsg[i]);
			}
			requeset_mutex_.Unlock();
			request_cond_.Signal();
		}

		if (pchBuf) {
			delete []pchBuf;
			pchBuf = 0;
		}
	}
}

void SocketShortServer::WaitMsg() {
	request_cond_.Wait();
	return;
}

bool SocketShortServer::GetMsg(int &socket_, string &msg_str) {	
	if (stop_flag_)
		return false;

	MutexGuard guard(&requeset_mutex_);

	if (request_queue_.empty())
		return false;
	
	_ReceivedMsg &msginfo = request_queue_.front();
	socket_ = msginfo.socket_;
	msg_str = msginfo.msg_str_;
	request_queue_.pop();
	return true;
}
} } }   ///< end of namespace ganji::util::net
