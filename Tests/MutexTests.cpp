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

#include "Thread.h"
 
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

#include <sstream>
#include <pthread.h>

class PThreadMutexPerfTest : public PerfTestBase {
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
        pthread_mutex_init(&mutex, 0);
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        counter = random_;
        for (int i = 0; i < rounds_; ++i) {
            pthread_mutex_lock(&mutex);
            counter++;
            pthread_mutex_unlock(&mutex);
        }
        return counter;
    }
    virtual void teardown(int& rounds_,int fourtytwo_,int random_) {
        pthread_mutex_destroy(&mutex);
    }
    std::string name() const {
        return "uncontended pthread_mutex";
    }
    pthread_mutex_t mutex;
    volatile int counter;
};

class ContendedPthreadMutexPerfTest : public PerfTestBase {
    virtual void setup(int& rounds_,int fourtytwo_,int random_) {
        pthread_mutex_init(&mutex, 0);
        for (int i = 0; i < (thread_num-1); ++i) {
            threads.push_back(new IncrementThread(this) );
        }
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            (*i)->waitForLaunch();
        }
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        rounds_ -= rounds_%(thread_num);
        rounds = rounds_/thread_num;
        number = 0;
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            (*i)->start();
        }
        for (int i = 0; i < rounds; ++i) {
            pthread_mutex_lock(&mutex);
            number++;
            pthread_mutex_unlock(&mutex);
        }
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            (*i)->join();
        }
        return number;
    }
    virtual void teardown(int& rounds_,int fourtytwo_,int random_) {
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            delete *i;
        }
        threads.clear();
        pthread_mutex_destroy(&mutex);
    }
    std::string name() const {
        std::ostringstream os;
        os << thread_num << " contended pthread_mutex";
        return os.str();
    }
    
    class IncrementThread: public Thread<ContendedPthreadMutexPerfTest*> {
    public:
        IncrementThread(ContendedPthreadMutexPerfTest* param) :
            Thread<ContendedPthreadMutexPerfTest*>(param) {}
        virtual void run( ContendedPthreadMutexPerfTest* const& param) {
            pthread_mutex_t& mutex = (param->mutex);
            volatile int* number = &(param->number);
            for (int i = 0; i < param->rounds; ++i) {
                pthread_mutex_lock(&mutex);
                number++;
                pthread_mutex_unlock(&mutex);
            }
        }
    };
    static const int thread_num = 2;
    pthread_mutex_t mutex;
    volatile int number;
    int rounds;
    std::vector<IncrementThread*> threads;
};

class PThreadRWLockReadLockPerfTest : public PerfTestBase {
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
        pthread_rwlock_init(&rwlock, 0);
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        counter = random_;
        for (int i = 0; i < rounds_; ++i) {
            pthread_rwlock_rdlock(&rwlock);
            counter++;
            pthread_rwlock_unlock(&rwlock);
        }
        return counter;
    }
    virtual void teardown(int& rounds_,int fourtytwo_,int random_) {
        pthread_rwlock_destroy(&rwlock);
    }
    std::string name() const {
        return "uncontended pthread_rwlock readlock";
    }
    pthread_rwlock_t rwlock;
    volatile int counter;
};

class ContendedPThreadRWLockReadLockPerfTest : public PerfTestBase {
public:
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
        pthread_rwlock_init(&rwlock, 0);
        for (int i = 0; i < (thread_num-1); ++i) {
            threads.push_back(new IncrementThread(this) );
        }
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            (*i)->waitForLaunch();
        }
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        rounds_ -= rounds_%(thread_num);
        rounds = rounds_/thread_num;
        number = 0;
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            (*i)->start();
        }
        for (int i = 0; i < rounds; ++i) {
            pthread_rwlock_rdlock(&rwlock);
            number++;
            pthread_rwlock_unlock(&rwlock);
        }
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            (*i)->join();
        }
        return number;
    }
    virtual void teardown(int& rounds_,int fourtytwo_,int random_) {
        for (std::vector<IncrementThread*>::const_iterator i = threads.begin(); i != threads.end(); ++i) {
            delete *i;
        }
        threads.clear();
        pthread_rwlock_destroy(&rwlock);
    }
    std::string name() const {
        std::ostringstream os;
        os << thread_num << " contended pthread_rwlock readlock";
        return os.str();
    }
    typedef class ContendedPThreadRWLockReadLockPerfTest Class;
    class IncrementThread: public Thread<Class*> {
    public:
        IncrementThread(Class* param) :
            Thread<Class*>(param) {}
        virtual void run( Class* const& param) {
            pthread_rwlock_t& rwlock = (param->rwlock);
            volatile int* number = &(param->number);
            int inc = 0;
            for (int i = 0; i < param->rounds; ++i) {
                pthread_rwlock_rdlock(&rwlock);
                inc = *number;
                pthread_rwlock_unlock(&rwlock);
            }
            number+=inc;
        }
    };
    static const int thread_num = 2;
    pthread_rwlock_t rwlock;
    volatile int number;
    int rounds;
    std::vector<IncrementThread*> threads;
};


PERFTEST_AUTOREGISTER(PThreadMutexPerfTest,(new PThreadMutexPerfTest()));
PERFTEST_AUTOREGISTER(ContendedPthreadMutexPerfTest,(new ContendedPthreadMutexPerfTest()));
PERFTEST_AUTOREGISTER(PThreadRWLockReadLockPerfTest,new PThreadRWLockReadLockPerfTest());
PERFTEST_AUTOREGISTER(ContendedPThreadRWLockReadLockPerfTest,new ContendedPThreadRWLockReadLockPerfTest());
