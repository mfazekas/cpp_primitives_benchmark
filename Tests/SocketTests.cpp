

#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

#include "SocketServer.h"

#include "ErrorCheck.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sstream>
#include <netdb.h>


class SendPerfTest : public PerfTestBase {
public:
    SendPerfTest(int sendsize) : sendsize(sendsize)
    {}
    
    
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
      startserver();
      clientsock = connectclientsocket();
      configureclientsocket(clientsock);
      
      std::string m = mode();
      int rc = ::send(clientsock,m.c_str(),m.size(),0);
      SOCK_ERRORCHECK(rc,"send");
    }
    
    virtual int connectclientsocket() {
      socklen_t size = sizeof(servaddr);
      int rc = ::getsockname(servsock, (struct sockaddr *)&servaddr,&size);
      SOCK_ERRORCHECK(rc,"getsockname");
      
      int clientsock = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      SOCK_ERRORCHECK(clientsock,"socket");
      rc = ::connect(clientsock,(struct sockaddr *)&servaddr,size);
      SOCK_ERRORCHECK(rc,"connect");
      return clientsock;
    }
    
    virtual void startserver() {
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
    }
    
    virtual void stopserver() {
      socketserver->stop();
      socketserver->join();
      delete socketserver;
      socketserver =0;
    }
    
    virtual void configureclientsocket(int socket) {}
    
    virtual std::string mode() {
        std::string mode = SocketServer::typemsg("/dev/null");
        return mode;
    }
    
    virtual void teardown(int roudnds_,int fourtytwo_,int random_) {
      int rc = ::close(clientsock);
      SOCK_ERRORCHECK(rc,"close");
      
      stopserver();
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
    RoundTripPerfTest(int sendsize, bool tcpnodelay) : 
        SendPerfTest(sendsize), tcpnodelay(tcpnodelay), remoteaddr()
    {}
    RoundTripPerfTest(const char* remoteaddr, int sendsize, bool tcpnodelay) : 
        SendPerfTest(sendsize), tcpnodelay(tcpnodelay), remoteaddr(remoteaddr)
    {}
    
    struct addrinfo* parse_host_port(const std::string& host_port) {
      std::string::size_type loc = host_port.find_last_of(':');
      MAKESURE(loc != std::string::npos, "host_port should contain a :");
      std::string host = host_port.substr(0,loc);
      std::string port = host_port.substr(loc+1);
      
      struct addrinfo hints, *res;
      memset(&hints, 0, sizeof(hints));
      hints.ai_family = PF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      int rc = getaddrinfo(host.c_str(),port.c_str(),&hints,&res);
      MAKESURE(rc == 0,"getaddrinfo failed!");
      return res;
    }
    
    virtual int connectclientsocket() {
      if (remoteaddr.size()) {
        addrinfo* ret = parse_host_port(remoteaddr);
        for (addrinfo* res = ret; res; res = res->ai_next) {
          int clientsock = ::socket(res->ai_family,res->ai_socktype,res->ai_protocol);
          SOCK_ERRORCHECK(clientsock,"socket");
          int rc = ::connect(clientsock,res->ai_addr,res->ai_addrlen);
          SOCK_ERRORCHECK(rc,"connect");
          freeaddrinfo(ret);
          return clientsock;
        }
        MAKESURE(false,"cannot resolve host port");
        return -1;
      } else {
        return SendPerfTest::connectclientsocket();
      }
    }
    virtual void startserver() {
      if (remoteaddr.size()) {
      } else {
        SendPerfTest::startserver();
      }
    }
    virtual void stopserver() {
      if (remoteaddr.size()) {
      } else {
        SendPerfTest::connectclientsocket();
      }
    }
    
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
        
        os << "send and receive (roundtrip) " << sendsize << " bytes on TCP ";
        if (remoteaddr.size()) {
            os << remoteaddr << ":";
        } else {
            os << "loopback";
        }
        os << " tcpnodelay:" << tcpnodelay;
        return os.str();
    }
    
    int defaultRounds() const { 
        return 1000; 
    }

    bool tcpnodelay;
    std::string remoteaddr;
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

PERFTEST_AUTOREGISTER(SendPerfTest1,new SendPerfTest(1));
PERFTEST_AUTOREGISTER(SendPerfTest16,new SendPerfTest(16));
PERFTEST_AUTOREGISTER(SendPerfTest128,new SendPerfTest(128));
PERFTEST_AUTOREGISTER(SendPerfTest1024,new SendPerfTest(1024));
PERFTEST_AUTOREGISTER(WritevPerfTest1,new WritevPerfTest(1));
PERFTEST_AUTOREGISTER(WritevPerfTest16,new WritevPerfTest(16));
PERFTEST_AUTOREGISTER(WritevPerfTest128,new WritevPerfTest(128));
PERFTEST_AUTOREGISTER(WritevPerfTest1024,new WritevPerfTest(1024));


PERFTEST_AUTOREGISTER(RoundTripPerfTest1,new RoundTripPerfTest(1,false));
PERFTEST_AUTOREGISTER(RoundTripPerfTest1024,new RoundTripPerfTest(1024,false));
PERFTEST_AUTOREGISTER(RoundTripPerfTest1NoNagle,new RoundTripPerfTest(1,true));
PERFTEST_AUTOREGISTER(RoundTripPerfTest1024NoNagle,new RoundTripPerfTest(1024,true));

static struct RegisterServerTests {
  RegisterServerTests() {
    if (getenv("SERVER_ADDR") != 0) {
      PERFTEST_REGISTER(new RoundTripPerfTest(getenv("SERVER_ADDR"),1,false));
      PERFTEST_REGISTER(new RoundTripPerfTest(getenv("SERVER_ADDR"),1024,false));
      PERFTEST_REGISTER(new RoundTripPerfTest(getenv("SERVER_ADDR"),1,true));
      PERFTEST_REGISTER(new RoundTripPerfTest(getenv("SERVER_ADDR"),1024,true));
    }
  }
} regsiterServerTests;

