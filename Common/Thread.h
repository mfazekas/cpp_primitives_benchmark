
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
