
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"
#include <stdio.h>
 
class SimpleIfPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = 0;
        for (int i = 0; i < rounds_; ++i) {
            if (fourtytwo_ < 42) {
                fourtytwo_ += 1;
                fprintf(stderr,"should not happen");
            }
        }
        return result;
    }
    std::string name() const {
        return "if branch";
    }
};

PERFTEST_AUTOREGISTER(SimpleIfPerfTest,new SimpleIfPerfTest());

