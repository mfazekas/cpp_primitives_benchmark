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

#include "Thread.h"
 
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

#include <sstream>

#ifdef __APPLE__
#include <libkern/OSAtomic.h>

typedef int32_t AtomicIncrementInt;
inline void AtomicIncrement(volatile AtomicIncrementInt* ptr) {
    OSAtomicIncrement32(ptr);
}
#define HASATOMICOPS 1
#else
#define HASATOMICOPS 0
#endif

#if HASATOMICOPS
class AtomicIncrementPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        AtomicIncrementInt result = random_;
        for (int i = 0; i < rounds_; ++i) {
            AtomicIncrement(&result);
        }
        return result;
    }
    std::string name() const {
        return "uncontended atomic_increment";
    }
};

class ContendedAtomicIncrementPerfTest : public PerfTestBase {
public:
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
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
            AtomicIncrement(&number);
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
    }
    std::string name() const {
        std::ostringstream os;
        os << thread_num << " contended atomic_increment";
        return os.str();
    }
    
    class IncrementThread: public Thread<ContendedAtomicIncrementPerfTest*> {
    public:
        IncrementThread(ContendedAtomicIncrementPerfTest* param) :
            Thread<ContendedAtomicIncrementPerfTest*>(param) {}
        virtual void run( ContendedAtomicIncrementPerfTest* const& param) {
            volatile AtomicIncrementInt* num = &(param->number);
            for (int i = 0; i < param->rounds; ++i) {
                AtomicIncrement(num);
            }
        }
    };
    static const int thread_num = 2;
    volatile AtomicIncrementInt number;
    int rounds;
    std::vector<IncrementThread*> threads;
};


PERFTEST_REGISTER(AtomicIncrement,new AtomicIncrementPerfTest());
PERFTEST_REGISTER(ContendedAtomicIncrementPerfTest,(new ContendedAtomicIncrementPerfTest()));
#endif

