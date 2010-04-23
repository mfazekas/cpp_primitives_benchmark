#ifndef SocketServer_H
#define SocketServer_H


#include "Thread.h"
#include "ErrorCheck.h"

#include <sstream>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/types.h>

class SocketServer {
public:
    SocketServer() : socket(-1),accepter(0) {
    }
    
    ~SocketServer() {
        for (size_t i = 0; i < serverthreads.size(); ++i) {
            delete serverthreads[i];
        }        
    }
    
    void start(int insocket) {
        socket = insocket;
        accepter = new AccepterThread(this);
        accepter->start();
    }
    
    void stop() {
        accepter->stop_loop(socket);

        accepter->join();
    }
    
    void join() {
        for (size_t i = 0; i < serverthreads.size(); ++i) {
            serverthreads[i]->join();
        }
    }
    
    static std::string typemsg(const char* type) {
        std::string result(type);
        short len = result.size();
        len = htons(len);
        std::string ret = std::string(reinterpret_cast<const char*>(&len),sizeof(len))+type;
        return ret;
    }
    
    class AccepterThread : public Thread<SocketServer*> {
    public:
        AccepterThread(SocketServer* server): 
            Thread<SocketServer*>(server) 
        {
            quit_flag = false;
        }
        
        void stop_loop(int socket) {
            quit_flag = true;
            int ret = ::close(socket);
            SOCK_ERRORCHECK(ret,"close");
        }
        
        virtual void run(SocketServer*const & param) {
            char cliaddr[1024];
            socklen_t len = sizeof(cliaddr);
            int accepted = 0;
            int sock = 0;
            
            fd_set rset;
            FD_ZERO(&rset);
            FD_SET(param->socket,&rset);
            
            fd_set eset;
            FD_ZERO(&eset);
            FD_SET(param->socket,&eset);
            
             
            while (1) {
                if (quit_flag) break;
                struct timeval tv = {0,100*1000};
                while ((::select(param->socket+1,&rset,NULL,&eset,&tv) == 0) && !quit_flag) {
                    FD_ZERO(&rset);
                    FD_SET(param->socket,&rset);
                    FD_ZERO(&eset);
                    FD_SET(param->socket,&eset);
                    tv.tv_sec = 0;
                    tv.tv_usec = 100*1000;
                }
                if (quit_flag) break;
                if ((sock = ::accept(param->socket,(struct sockaddr*)&cliaddr,&len)) > 0) {
                    accepted++;
                    ServerThread* thread = new ServerThread(sock);
                    thread->start();
                    param->serverthreads.push_back(thread);
                }

            }
        }

        volatile bool quit_flag;
    };
    
    class ServerThread : public Thread<int> {
    public:
        ServerThread(const int& param) : 
            Thread<int>(param) 
        {}
        
        std::string receivetypemsg(int socket) {
            char buffer[4096];
            short len = 0;
            int ret = ::recv(socket,reinterpret_cast<char*> (&len), sizeof(len), MSG_WAITALL);
            SOCK_ERRORCHECK(ret-sizeof(len),"recv(&len,MSG_WAITALL)");
            len = htons(len);
            MAKESURE(size_t(len) < sizeof(buffer),"len is too large");
            ret = ::recv(socket,buffer,len,MSG_WAITALL);
            MAKESURE(ret == len,"recv MSG_WAITALL");
            return std::string(buffer,len);
        }
        
        void null_loop(int socket) {
            int result = 0;
            int flags = 0;
            int received;
            while ((received = ::recv(socket,buffer,sizeof(buffer),flags)) > 0) {
                result += received;
            } 
        }

        void echo_loop(int socket) {
            int result = 0;
            int flags = 0;
            int received;
            while ((received = ::recv(socket,buffer,sizeof(buffer),flags)) > 0) {
                ::send(socket,buffer,received,0);
                result += received;
            }
        }
        
        void set_tcpnodelay(int socket, bool tcpnodelay)
        {
          int flag = tcpnodelay;
          int rc = ::setsockopt(socket,IPPROTO_TCP,TCP_NODELAY,(char*)&flag,sizeof(flag));
          SOCK_ERRORCHECK(rc,"::setsockopt - TCP_NODELAY");
        }
        
        std::string readandapplyparams(int socket) {
            std::string type_and_params = receivetypemsg(socket);
            std::istringstream is(type_and_params);
            std::string type;
            is >> type;
            std::string paramstr;
            while(is >> paramstr) {
                if (paramstr == std::string("tcp_nodelay")) {
                    bool value;
                    is >> value;
                    set_tcpnodelay(socket,value);
                }
            }
            return type;
        }
        
        virtual void run(const int& param) {
            int socket = param;
            std::string type = readandapplyparams(socket);
            if (type == std::string("/dev/null")) {
                null_loop(socket);
            } else if (type == std::string("echo")) {
                echo_loop(socket);
            }
            int rc = ::close(socket);
            SOCK_ERRORCHECK(rc,"close");
        }
        char buffer[4*1024];
    };
    
    int socket;
    AccepterThread* accepter;
    std::vector<ServerThread*> serverthreads;
};

#endif
