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

#include "PerfTestRegistry.h"
#include "PerfTestBase.h"
#include <stdlib.h> 
#include <sstream>

template <int size>
class MallocFreeCallPerfTest : public PerfTestBase {
public:
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = 0;
        for (int i = 0; i < rounds_; ++i) {
            void* x  = malloc(fourtytwo_-42+size);
            result += reinterpret_cast<ptrdiff_t>(x);
            free(x);
        }
        return result;
    }
    std::string name() const {
        std::ostringstream os;
        os << "mallocfree-" << size;
        return os.str();
    }    
};

template <int size,int batchsize>
class LotMallocFreeCallPerfTest : public PerfTestBase {
public:
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = 0;
        rounds_ -= rounds_ % batchsize;
        for (int i = 0; i < rounds_/batchsize; ++i) {
            void* ptrs[batchsize];
            for (int j =0; j < batchsize; ++j) {
                ptrs[j] = malloc(fourtytwo_-42+size);
            }
            result += reinterpret_cast<ptrdiff_t>(ptrs[0]);
            for (int j =0; j < batchsize; ++j) {
                free(ptrs[j]);
            }
        }
        return result;
    }
    std::string name() const {
        std::ostringstream os;
        os << "mallocfree-" << size << "batch-" << batchsize;
        return os.str();
    }    
};
  
PERFTEST_AUTOREGISTER(MallocPerfTest1,new MallocFreeCallPerfTest<10>());
PERFTEST_AUTOREGISTER(MallocPerfTest2,new MallocFreeCallPerfTest<100>());
PERFTEST_AUTOREGISTER(MallocPerfTest3,new MallocFreeCallPerfTest<1024>());
PERFTEST_AUTOREGISTER(MallocPerfTest4,new MallocFreeCallPerfTest<8192>());

PERFTEST_AUTOREGISTER(MallocFreePerfTest1,(new LotMallocFreeCallPerfTest<10,1024>()));
PERFTEST_AUTOREGISTER(MallocFreePerfTest2,(new LotMallocFreeCallPerfTest<100,1024>()));
PERFTEST_AUTOREGISTER(MallocFreePerfTest3,(new LotMallocFreeCallPerfTest<1024,1024>()));
