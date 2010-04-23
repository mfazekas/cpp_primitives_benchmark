
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
