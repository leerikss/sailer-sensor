#include "socketclient.h"

socketclient::socketclient(const Config& cfg) : logger(cfg)
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
      logger.error("Could not create socket");
    }
  }

  server.sin_addr.s_addr = inet_addr( address.c_str() );

  if( server.sin_addr.s_addr == (in_addr_t)(-1) )
  {
    logger.error("Socket connection failed. Bad IP address");
    return false;
  }
  
  server.sin_family = AF_INET;
  server.sin_port = htons( port );
     
  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    logger.error("Socket connection failed.");
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
    logger.error("Socket send_data failed. Data: "+data);
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
    logger.error("Unable to shutdown socket");
  sock = -1;
}
