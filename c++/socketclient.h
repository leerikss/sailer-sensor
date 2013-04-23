#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
 
using namespace std;
 
/**
   TCP Client class
*/
class socketclient
{
private:
  int sock;
  std::string address;
  int port;
  struct sockaddr_in server;
     
public:
  socketclient();
  bool conn(string, int);
  bool send_data(string data);
  void close(void);
};
 
socketclient::socketclient()
{
  sock = -1;
  port = 0;
  address = "";
}
 
/**
   Connect to a host on a certain port number
*/
bool socketclient::conn(string address , int port)
{
  //create socket if it is not already created
  if(sock == -1)
  {
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
      cerr << "Could not create socket" << endl;
    }
  }

  server.sin_addr.s_addr = inet_addr( address.c_str() );

  if( server.sin_addr.s_addr == (in_addr_t)(-1) )
  {
    cerr << "Bad IP address" << endl;
    return false;
  }
  
  server.sin_family = AF_INET;
  server.sin_port = htons( port );
     
  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    cerr << "connect failed. Error" << endl;
    return false;
  }
     
  return true;
}
 
/**
   Send data to the connected host
*/
bool socketclient::send_data(string data)
{
  //Send some data
  if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
  {
    cerr << "Send failed : ";
    return false;
  }
  return true;
}
 
/**
   Receive data from the connected host
*/
void socketclient::close(void)
{
  if( shutdown(sock,SHUT_RDWR) )
    cerr << "Unable to shutdown socket" << endl;
  sock = -1;
}
