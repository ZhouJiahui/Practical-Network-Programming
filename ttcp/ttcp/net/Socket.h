#ifndef TTCP_NET_SOCKET_H_
#define TTCP_NET_SOCKET_H_

#include <algorithm>

#include "basic.h"

class InetAddress;

class Socket : noncopyable
{
public:
  Socket();
  Socket(int fd);
  ~Socket();
  Socket(Socket &&rhs);
  Socket& operator = (Socket &rhs);
  void swap(Socket &rhs);
  int fd() const
  { return sockfd_;}
  void bindOrDie(const InetAddress&);
  void listenOrDie();
  int connect(const InetAddress&);
  void shutdownWrite();
  void close();
  void setReuseAddr(bool on);
  void setTcpNoDelay(bool on);
  int read(void*, int);
  int write(const void*, int);
  InetAddress getLocalAddr() const;
  InetAddress getPeerAddr() const;

private:
  int sockfd_;
};

#endif
