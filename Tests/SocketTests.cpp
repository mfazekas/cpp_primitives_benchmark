/*
Copyright (c) 2007-, Miklós Fazekas
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


#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

#include "SocketServer.h"

#include "ErrorCheck.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sstream>


class SendPerfTest : public PerfTestBase {
public:
    SendPerfTest(int sendsize) : sendsize(sendsize)
    {}
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
      servsock = ::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
      SOCK_ERRORCHECK(servsock,"socket");
      
      memset(&servaddr, 0, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /*INADDR_ANY*/
      servaddr.sin_port = 0;
      int rc;
      rc = ::bind(servsock,(struct sockaddr *)&servaddr,sizeof(servaddr));
      SOCK_ERRORCHECK(rc,"bind");
      rc = ::listen(servsock, 3);
      SOCK_ERRORCHECK(rc,"listen");
      
      socketserver = new SocketServer();
      socketserver->start(servsock);

      clientsock = connectclientsocket(servsock);
      configureclientsocket(clientsock);
      std::string m = mode();
      rc = ::send(clientsock,m.c_str(),m.size(),0);
      SOCK_ERRORCHECK(rc,"send");
    }
    
    virtual int connectclientsocket(int servsock) {
      socklen_t size = sizeof(servaddr);
      int rc = ::getsockname(servsock, (struct sockaddr *)&servaddr,&size);
      SOCK_ERRORCHECK(rc,"getsockname");
      
      int clientsock = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      SOCK_ERRORCHECK(clientsock,"socket");
      rc = ::connect(clientsock,(struct sockaddr *)&servaddr,size);
      SOCK_ERRORCHECK(rc,"connect");
      return clientsock;
    }
    
    virtual void configureclientsocket(int socket) {}
    
    virtual std::string mode() {
        std::string mode = SocketServer::typemsg("/dev/null");
        return mode;
    }
    
    virtual void teardown(int roudnds_,int fourtytwo_,int random_) {
      int rc = ::close(clientsock);
      SOCK_ERRORCHECK(rc,"close");
      
      socketserver->stop();
      socketserver->join();
      delete socketserver;
      socketserver =0;
    }
    
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = random_;
        for (int i = 0; i < rounds_; ++i) {
            result += ::send(clientsock,sendbuffer,sendsize,0);
        }
        return result;
    }

    std::string name() const {
        std::ostringstream os;
        os << "send " << sendsize << " bytes on TCP loopback";
        return os.str();
    }

    struct sockaddr_in servaddr;
    int servsock;
    int sendsize;
    char sendbuffer[1024*1024];
    
    int clientsock;
    
    SocketServer* socketserver;
};

class RoundTripPerfTest : public SendPerfTest {
public:
    RoundTripPerfTest(int sendsize, bool tcpnodelay) : SendPerfTest(sendsize), tcpnodelay(tcpnodelay)
    {}
    virtual void configureclientsocket(int socket) {
        int flag = tcpnodelay;
        int rc = ::setsockopt(socket,IPPROTO_TCP,TCP_NODELAY,(char*)&flag,sizeof(flag));
        SOCK_ERRORCHECK(rc,"::setsockopt - TCP_NODELAY");
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = random_;
        for (int i = 0; i < rounds_; ++i) {
            result += ::send(clientsock,sendbuffer,sendsize,0);
            int rc = ::recv(clientsock,sendbuffer,sendsize,MSG_WAITALL);
            SOCK_ERRORCHECK(rc,"recv");
        }
        return result;
    } 
    virtual std::string mode() {
        std::ostringstream os;
        os << "echo";
        os << " tcp_nodelay " << tcpnodelay;
        std::string mode = SocketServer::typemsg(os.str().c_str());
        return mode;
    }
    std::string name() const {
        std::ostringstream os;
        os << "send and receive (roundtrip) " << sendsize << " bytes on TCP loopback tcpnodelay:" << tcpnodelay;
        return os.str();
    }
    int defaultRounds() const { 
        return 1000; 
    }
    bool tcpnodelay;
};

#include <sys/uio.h>

class WritevPerfTest : public SendPerfTest {
public:
    WritevPerfTest(int sendsize) : SendPerfTest(sendsize)
    {}
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = random_;
        for (int i = 0; i < rounds_; ++i) {
            struct iovec iov = {sendbuffer,sendsize};
            result += ::writev(clientsock,&iov,1);
        }
        return result;
    }
    std::string name() const {
        std::ostringstream os;
        os << "writev " << sendsize << " bytes on TCP loopback";
        return os.str();
    }
};

PERFTEST_REGISTER(SendPerfTest1,new SendPerfTest(1));
PERFTEST_REGISTER(SendPerfTest16,new SendPerfTest(16));
PERFTEST_REGISTER(SendPerfTest128,new SendPerfTest(128));
PERFTEST_REGISTER(SendPerfTest1024,new SendPerfTest(1024));
PERFTEST_REGISTER(WritevPerfTest1,new WritevPerfTest(1));
PERFTEST_REGISTER(WritevPerfTest16,new WritevPerfTest(16));
PERFTEST_REGISTER(WritevPerfTest128,new WritevPerfTest(128));
PERFTEST_REGISTER(WritevPerfTest1024,new WritevPerfTest(1024));

PERFTEST_REGISTER(RoundTripPerfTest1,new RoundTripPerfTest(1,false));
PERFTEST_REGISTER(RoundTripPerfTest1024,new RoundTripPerfTest(1024,false));
PERFTEST_REGISTER(RoundTripPerfTest1NoNagle,new RoundTripPerfTest(1,true));
PERFTEST_REGISTER(RoundTripPerfTest1024NoNagle,new RoundTripPerfTest(1024,true));

