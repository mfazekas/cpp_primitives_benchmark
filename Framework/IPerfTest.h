#ifndef IPerfTestHeader
#define IPerfTestHeader
 
#include <string>

class IPerfTest {
public:
    virtual void setup(int& rounds_,int fourtytwo_,int random_) = 0;
    virtual int perform (int& rounds_,int fourtytwo_,int random_) = 0;
    virtual void teardown(int rounds,int fourtytwo_,int random_) = 0;
    
    virtual std::string name() const  = 0;
    
    virtual std::string unit() const = 0;
    
    virtual int defaultRounds() const = 0;
    
    virtual ~IPerfTest();   
};

#endif // IPerfTestHeader
