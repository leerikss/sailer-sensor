#ifndef socketclient_h
#define socketclient_h
#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
#include<libconfig.h++>
#include"log.h"

using namespace libconfig; 
using namespace std;
 
/**
   TCP Client class
*/
class socketclient
{
public:
  socketclient(const Config& cfg);

  bool conn(string, int);
  bool send_data(string data);
  void close(void);

private:
  int sock;
  struct sockaddr_in server;
  std::string address;
  int port;
  log logger;
};

#endif
