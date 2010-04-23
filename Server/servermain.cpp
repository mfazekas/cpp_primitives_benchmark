/*
Copyright (c) 2007-, cpp_primitives_benchmark contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the <organization>.
4. Neither the name of the <organization> nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
