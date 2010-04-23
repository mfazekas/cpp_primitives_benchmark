 
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"
#include <sstream>

template <int size>
class MemCpyPerfTest : public PerfTestBase {
public:
    virtual int perform(int& rounds_,int fourtytwo_,int random_) {
        int32_t result;
        for (int i = 0; i < rounds_; ++i) {
            int idx = i*(fourtytwo_-42);
            memcpy(src+idx,dest+idx,size);
        }
        return result;
        
    }
    
    std::string name() const {
        std::ostringstream os;
        os << "MemCpyPerfTest<" << size << ">";
        return os.str();
    }
    
    char dest[42+size];
    char src[42+size];
};

PERFTEST_AUTOREGISTER(MemCpyPerfTest1,new MemCpyPerfTest<10>());
PERFTEST_AUTOREGISTER(MemCpyPerfTest2,new MemCpyPerfTest<100>());
PERFTEST_AUTOREGISTER(MemCpyPerfTest3,new MemCpyPerfTest<1024>());
PERFTEST_AUTOREGISTER(MemCpyPerfTest4,new MemCpyPerfTest<4096>());
PERFTEST_AUTOREGISTER(MemCpyPerfTest5,new MemCpyPerfTest<4*4096>());
