/** 
 * @Copyright(c)  2010 Ganji Inc.
 * @file          ganji/util/net/socket_stream.h
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

#ifndef _GANJI_UTIL_NET_SOCKET_STREAM_H_
#define _GANJI_UTIL_NET_SOCKET_STREAM_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "ganji/ganji_global.h"


namespace ganji { namespace util { namespace net {

using namespace std;

class SocketStream {
 public:
  SocketStream() : socket_ptr_(NULL), is_close_when_delete_(false) {
  }
  SocketStream(int *socket, bool is_close=true) : socket_ptr_(socket), is_close_when_delete_(is_close) {
  }
  SocketStream(const SocketStream &obj) {
    this->socket_ptr_ = obj.socket_ptr_;
    this->is_close_when_delete_ = obj.is_close_when_delete_;
  }
  ~SocketStream() {
    if (!is_close_when_delete_)
      return;
    if (socket_ptr_ == NULL)
      return;
    close(*socket_ptr_);
    *socket_ptr_ = -1;
  }
  void SetFD(int *socket) {
    this->socket_ptr_ = socket;
  }
  int GetFD() {
    if (socket_ptr_ == NULL)
      return -1;
    return *socket_ptr_;
  }

  int raw_send(const void *buf, uint32_t len);
  int raw_recv(void *buf, uint32_t len);
  bool send_n(const void *buf, uint32_t n);
  bool recv_n(void * buf, uint32_t n);
  void lingering_close();

protected:
	bool  is_close_when_delete_;
	int   *socket_ptr_;
};

inline int SocketStream::raw_send(const void *buf, uint32_t len) {
	if ((NULL == socket_ptr_) || (*socket_ptr_ < 0))
		return -1;

	int dida = 0;
	int count = 0;
	do {
		count = send(*socket_ptr_, (const char *)buf, len, 0);
		if (5 == (dida++))
			return -1;
	} while (0 == count);

	if (count < 0) {
		return -1;
	}
	return count;
}

inline int SocketStream::raw_recv(void *buf, uint32_t len) {
	if ((NULL == socket_ptr_) || (*socket_ptr_ < 0))
		return -1;

	int dida = 0;
	int count = 0;
	do {
		count = recv(*socket_ptr_, (char *)buf, len, 0);
		if (5 == (dida++))
			return -1;
	} while (0 == count);

	if (count < 0) {
		return -1;
	}
	return count;
}

inline bool SocketStream::send_n(const void *buf, uint32_t n) {
	const char *p = (const char *)buf;
	while (n > 0) {
		int s = raw_send(p, n);
		if (s < 0)
      return false;
		n -= s;
		p += s;
	}
	return true;
}

inline bool SocketStream::recv_n(void *buf, uint32_t n) {
	char *p = (char *)buf;

	while (n > 0) {
		int s = raw_recv(p, n);
		if (s < 0)
      return false;
		n -= s;
		p += s;
	}
	return true;
}

inline void SocketStream::lingering_close() {
  if ((NULL == socket_ptr_) || (*socket_ptr_ < 0))
    return;
  
  int lsd = *socket_ptr_;
	char dummybuf[512];
	struct timeval tv;
	fd_set lfds;
	int select_rv;

	shutdown(lsd, 1);

	FD_ZERO(&lfds);
	do {
		FD_SET(lsd, &lfds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		select_rv = select(lsd + 1, &lfds, NULL, NULL, &tv);
	} while ((select_rv > 0) && (read(lsd, dummybuf, 512) > 0));
	close(lsd);
}
} } }   ///< end of namespace ganji::util::net
#endif  ///< _GANJI_UTIL_NET_SOCKET_STREAM_H_
