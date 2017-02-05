#ifndef TTCP_NET_ACCEPTOR_H_
#define TTCP_NET_ACCEPTOR_H_

#include <memory>

#include "basic.h"
#include "Socket.h"

class InetAddress;
class TcpStream;
typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class Acceptor : noncopyable
{
public:
  explicit Acceptor(const InetAddress&);
  ~Acceptor();
//  Acceptor(Acceptor&&) = default;
//  Acceptor& operator=(Acceptor&&) = default;
  TcpStreamPtr accept();

private:
  Socket listenSock_;
};

#endif
