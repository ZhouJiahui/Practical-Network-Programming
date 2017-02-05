#include <stdio.h>
#include <sys/socket.h>

#include "TcpStream.h"
#include "Acceptor.h"

Acceptor::Acceptor(const InetAddress &addr)
{
  listenSock_.setReuseAddr(true);
  listenSock_.setTcpNoDelay(true);
  listenSock_.bindOrDie(addr);
  listenSock_.listenOrDie();
}

Acceptor::~Acceptor()
{
  listenSock_.close();
}

TcpStreamPtr Acceptor::accept()
{
  int sockfd = ::accept(listenSock_.fd(), NULL, NULL);
  if (sockfd >= 0)
  {
    return TcpStreamPtr(new TcpStream(Socket(sockfd)));
  }
  else
  {
    perror("Acceptor::accept");
    return TcpStreamPtr();
  }
}
