#ifndef SocketServer_H
#define SocketServer_H
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

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "Thread.h"
#include "ErrorCheck.h"
#include <sstream>
#include <sys/types.h>
#include <signal.h>

static int alarmed = 0;
static void alarm_handler(int sig) { alarmed = 1; }
 
class SocketServer {
public:
    SocketServer() : socket(-1),accepter(0) {
    }
    
    ~SocketServer() {
        for (int i = 0; i < serverthreads.size(); ++i) {
            delete serverthreads[i];
        }        
    }
    
    void start(int insocket) {
        socket = insocket;
        accepter = new AccepterThread(this);
        accepter->start();
    }
    
    void stop() {
        accepter->stop_loop();
        int ret = ::close(socket);
        SOCK_ERRORCHECK(ret,"close");
        accepter->join();
    }
    
    void join() {
        for (int i = 0; i < serverthreads.size(); ++i) {
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
        
        void stop_loop() {
            quit_flag = true;
            signal(SIGINT, alarm_handler);
            // hack,hack,hack 
            // TODO: set quit flag then just connect to wake up
            usleep(1);
            
            kill(getpid(),SIGINT);
        }
        
        virtual void run(SocketServer*const & param) {
            char cliaddr[1024];
            socklen_t len = sizeof(cliaddr);
            int accepted = 0;
            int sock = 0;
            while (!quit_flag && ((sock = ::accept(param->socket,(struct sockaddr*)&cliaddr,&len)) > 0)) {
                accepted++;
                ServerThread* thread = new ServerThread(sock);
                thread->start();
                param->serverthreads.push_back(thread);
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
            MAKESURE(len < sizeof(buffer),"len is too large");
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