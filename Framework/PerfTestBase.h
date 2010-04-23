#ifndef PerfTestBaseHeader
#define PerfTestBaseHeader
 
#include "IPerfTest.h"

#define BEGIN_REPEAT(count)  for(int loop_i = 0; loop_i < count; ++loop_i) 
#define END_REPEAT           

class PerfTestBase : public IPerfTest {
public:
    virtual void setup(int& rounds_,int fourtytwo_,int random_);
    virtual void teardown(int rounds,int fourtytwo_,int random_);
    
    virtual std::string unit() const;
    
    virtual int defaultRounds() const;
};
 
#endif // PerfTestBaseHeader

