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

#ifndef Thread_H
#define Thread_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include <string.h>

#define PTHREAD_CHECK(res) if (int err = res) { fprintf(stderr,#res" failed:%d errno:%d strerror:%s",err,errno,strerror(errno)); exit(1); }
template <typename Param>
class Thread {
public:
    Thread(const Param& param) : in_wait_for_start(false),started(false),param(param) {
        PTHREAD_CHECK(pthread_create(&thread,
                         0,
                         run,
                         this));
    }
    static void* run(void* _this) {
        return reinterpret_cast<Thread*>(_this)->do_run();
    }
    virtual void* do_run() {
        wait_for_start();
        run(param);
        return 0;
    }
    virtual void wait_for_start() {
        in_wait_for_start  = true;
        while (!started) {}
    }
    void start() {
        waitForLaunch();
        started = true;
    }
    void waitForLaunch() {
        while(!in_wait_for_start) {}
    }
    void join() {
        if (thread) {
          void* result = 0;
          pthread_join(thread,&result);
          started = false;
          thread = 0;
        }
    }
    virtual ~Thread() {
        if (started) join();
    }
    virtual void run(const Param& param) = 0;
private:
    volatile bool in_wait_for_start;
    volatile bool started;
    pthread_t thread;
    Param param;
};

#endif
