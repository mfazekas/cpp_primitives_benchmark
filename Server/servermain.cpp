
#include "SocketServer.h"
#include "ErrorCheck.h"
#include <sys/socket.h>


int main(int argc,const char* argv[]) {
  int servsock = ::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  SOCK_ERRORCHECK(servsock,"socket");
      
  sockaddr_in servaddr;  
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = 0;
  int rc;
  rc = ::bind(servsock,(struct sockaddr *)&servaddr,sizeof(servaddr));
  SOCK_ERRORCHECK(rc,"bind");
  rc = ::listen(servsock, 3);
  SOCK_ERRORCHECK(rc,"listen");
  
  socklen_t size = sizeof(servaddr);
  rc = ::getsockname(servsock, (struct sockaddr *)&servaddr,&size);
  SOCK_ERRORCHECK(rc,"getsockname");
  
  SocketServer server;
  server.start(servsock);
  fprintf(stdout,"Server listening on port:%d\n",htons(servaddr.sin_port));
  while(1) {}
  server.stop();
  return 0;
}
