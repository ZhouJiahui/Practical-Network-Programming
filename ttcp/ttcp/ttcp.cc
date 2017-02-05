#include <string>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#include "net/Acceptor.h"
#include "net/TcpStream.h"
#include "net/InetAddress.h"

typedef enum
{
  kUnknown,
  kReceive,
  kTransmit,  
  kHelp
} State;

struct SessionMessage
{
  int32_t count;
  int32_t size;
} __attribute__ ((__packed__));

struct PayLoadMessage
{
  int32_t length;
  char  data[0];
};

struct Options
{
  State state;
  int32_t count;
  int32_t size;
  std::string ip;
  int port;
};


double now()
{
  timeval tv = { 0, 0 };
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

bool parseCommandLine(int argc, char **argv, Options  &options)
{
  options.state = kUnknown;
  for(int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-h") && i == 1 && i == argc-1)
    {
      options.state = kHelp;
    }
    else if ((!strcmp(argv[i], "-t") || !strcmp(argv[i], "-r")) && i == 1)
    {
      if (!strcmp(argv[i], "-t"))
      {
        options.state = kTransmit;
        options.count = 2048;
        options.size = 8196;
      }
      else
      {
        options.state = kReceive;
        options.ip = std::string("127.0.0.1");
      }
      options.port = 2017;
    }
    else if (!strcmp(argv[i], "-i") && i != argc-1)
    {
      options.ip = std::string(argv[i+1]);
      ++i;
    }
    else if (!strcmp(argv[i], "-p") && i != argc-1)
    {
      int port = atoi(argv[i+1]);
      options.port = port;
      ++i;
    }
    else if (!strcmp(argv[i], "-c") && i != argc-1)
    {
      int32_t count = atoi(argv[i+1]);
      options.count = count;
      ++i;
    }
    else if (!strcmp(argv[i], "-s") && i != argc-1)
    {
      int size = atoi(argv[i+1]);
      options.size = size;
      ++i;
    }
    else 
    {
      printf("Unknown option: %s.\n", argv[i]); 
      return false;
    }
  } 
  if (options.state == kUnknown)
  {
    return false;
  }
  return true;
}

void help()
{
  printf("usage: ttcp [options]\n");
  printf("example:\n");
  printf("ttcp -t -i 127.0.0.1 -p 2017 -c 2048 -s 8196\n");
  printf("ttcp -r -p 2048\n");
  printf("options:\n");
  printf("  -h            help\n");
  printf("  -r            receive\n");
  printf("  -t            transmit\n");
  printf("  -i=127.0.0.1  ip\n");
  printf("  -p=2017       port\n");
  printf("  -c=2048       count\n");
  printf("  -s=8196       size\n");
}

void transmit(const Options &options)
{
  InetAddress addr(options.port);
  Acceptor acceptor(addr);
  while (true)
  {
    TcpStreamPtr stream = acceptor.accept();
    if (!stream)
    {
      continue;
    }
    SessionMessage sessionMsg = {0, 0};
    int nread;
    // read sessionMsg
    nread = stream->receiveAll(&sessionMsg, sizeof sessionMsg);
    assert( nread == sizeof sessionMsg);
    sessionMsg.count = ntohl(sessionMsg.count);
    sessionMsg.size = ntohl(sessionMsg.size);
    printf("sessionMsg.count=%d  sessionMsg.size=%d\n",sessionMsg.count,sessionMsg.size);  
    const int total_length = static_cast<int>(sizeof(int32_t) + sessionMsg.size);
    PayLoadMessage *payLoadMsg = static_cast<PayLoadMessage*>(::malloc(total_length));
    for(int i = 0; i < sessionMsg.count; ++i)
    {
      // read payLoadMsg length
      payLoadMsg->length = 0;
      nread = stream->receiveAll(&payLoadMsg->length, sizeof payLoadMsg->length);
      assert(nread == sizeof payLoadMsg->length);
      // read payLoadMsg data
      payLoadMsg->length=ntohl(payLoadMsg->length);
      nread = stream->receiveAll(&payLoadMsg->data, payLoadMsg->length);
      assert(nread == payLoadMsg->length);
      // write ack
      int ack;
      ack = static_cast<int>(sizeof (payLoadMsg->length) + payLoadMsg->length);
      ack = htonl(ack);
      int nwrite;
      nwrite = stream->sendAll(&ack, sizeof ack);
      assert(nwrite == sizeof ack);
    }
    ::free(payLoadMsg);
  }
}

void receive(const Options &options)
{
  InetAddress serverAddr(options.ip, options.port);
  TcpStreamPtr stream = TcpStream::connect(serverAddr);
  // write sessionMsg
  SessionMessage sessionMsg = {0, 0};
  sessionMsg.count = htonl(options.count);
  sessionMsg.size = htonl(options.size);
  int nwrite;
  nwrite = stream->sendAll(&sessionMsg,sizeof sessionMsg);
  assert(nwrite == sizeof sessionMsg);
  const int total_length = static_cast<int>(sizeof(int32_t) + options.size);
  PayLoadMessage *payLoadMsg = static_cast<PayLoadMessage*>(::malloc(total_length));
  payLoadMsg->length = htonl(options.size);
  for(int i = 0; i < options.count; ++i)
  {
    payLoadMsg->data[i] = "1234567890ABCDEF"[i%16];
  } 
  double total_mb = options.count * options.count * 1.0 / 1024 / 1024;
  printf("%.3f MiB in total\n", total_mb);
  double start = now();  
  for(int i = 0; i < options.count; ++i)
  {
    // write payLoadMsg
    nwrite = stream->sendAll(payLoadMsg, total_length);
    assert(nwrite == total_length);
    // read ack
    int ack;
    int nread;
    nread = stream->receiveAll(&ack, sizeof ack);
    assert(nread == sizeof ack);
    ack = ntohl(ack);
    assert(ack == total_length);
  }
  double elapsed = (now() - start) / 1000000.0;
  printf("%.3lf seconds\n%.3lf MiB/s.\n", elapsed, total_mb / elapsed);
}

int main(int argc, char **argv)
{
  Options options;
  if (parseCommandLine(argc, argv, options))
  {
    if (options.state == kHelp)
    {
      help();
    }
    else if (options.state == kTransmit)
    {
      transmit(options);
    }
    else if (options.state == kReceive)
    {
      receive(options);
    }
  }
  else
  {
    printf("input parameter error.\n");
    printf("use -h for more information.\n");
  }
  return 0;
}
