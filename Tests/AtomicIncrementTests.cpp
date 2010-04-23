
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


PERFTEST_AUTOREGISTER(AtomicIncrement,new AtomicIncrementPerfTest());
PERFTEST_AUTOREGISTER(ContendedAtomicIncrementPerfTest,(new ContendedAtomicIncrementPerfTest()));
#endif

