#include <string>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum
{
  kUnknown,
  kReceive,
  kTransmit,  
  kHelp
}State;

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
}

void receive(const Options &options)
{
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
