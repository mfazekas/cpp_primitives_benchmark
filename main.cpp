#import <Foundation/Foundation.h>

#include <string>
#include <iostream>
#include <sstream>

#include "IPerfTest.h"
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

class RegisterAddPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = random_;
        for (int i = 0; i < rounds_; ++i) {
            result += random_;
        }
        return result;
    }
    std::string name() const {
        return "register int::operator+=";
    }
};

class VirtualCallPerfTest : public PerfTestBase {
public:
    class Base {
    public:
        virtual void call() = 0;
    };
    
    class Base1 : public Base {
    public:
        virtual void call() {}
    };
    class Base2 : public Base {
    public:
        virtual void call() {}
    };
    

    virtual void setup(int& rounds_,int fourtytwo_,int random_) {
        base = new Base2();
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        for (int i = 0; i < rounds_; ++i) {
            base->call();
        }
        return 0;
    }
    virtual void teardown(int rounds_,int fourtytwo_,int random_) {
        delete base; base = 0;
    }
    std::string name() const {
        return "c++virtualmethodcall";
    }    
    Base* base;
};


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


class VolatileAddPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int32_t result;
        for (int i = 0; i < rounds_; ++i) {
            result += random_;
        }
        return result;
    }
    std::string name() const {
        return "volatile int::operator+=";
    }
    volatile int result;
};

#include <sys/time.h>

class Time {
public:
    Time(double seconds) : seconds(seconds) {}
    double asSeconds() const { return seconds; }
    
    static Time usec(double useconds) { return Time(useconds/1000000.0); }
    
private:
    double seconds;
};

class Random {
public:
    static int generate() { return 123; }
};

class MeasureTime {
public:
    MeasureTime(Time* elapsedTime) : elapsedTime(elapsedTime) {
        gettimeofday(&startTime, NULL);
    }
    ~MeasureTime() {
        gettimeofday(&endTime, NULL);
        *elapsedTime = Time::usec(1000000.0 * (endTime.tv_sec - startTime.tv_sec) + double(endTime.tv_usec - startTime.tv_usec));
    }
private:
    Time* elapsedTime;
    struct timeval startTime;
    struct timeval endTime;
};

Time benchmark(IPerfTest* perfTest_,int rounds) {
    Time elapsedSeconds(0);
    int random = Random::generate();
    perfTest_->setup(rounds,42,random);
    {
        MeasureTime time(&elapsedSeconds); 
        perfTest_->perform(rounds, 42, random);
    }
    perfTest_->teardown(rounds,42,random);
    return elapsedSeconds;
}

#include <vector> 

    // minimal seconds for relaible benchmark
double minSecondsTime = 0.01;
    // optiomal seconds for relaible benchmark
double optSecondsTime = 0.1;

class IReporter {
public:
    virtual void report(IPerfTest& perfTest, int rounds,const Time& time) = 0;
};

class Reporter : public IReporter {
public:
    virtual void report(IPerfTest& perfTest,int rounds,const Time& time) {
        std::cerr << "name:" << perfTest.name() << ": " << double(rounds)/time.asSeconds() << " " << perfTest.unit() << "/seconds" << std::endl;
    }
};

void benchmarkAll(const std::vector<IPerfTest*>& perfTests_,IReporter& reporter)
{
    for (std::vector<IPerfTest*>::const_iterator i = perfTests_.begin(); i != perfTests_.end(); ++i) {
        int rounds = (*i)->defaultRounds();
        Time time = benchmark(*i, rounds);
        if (time.asSeconds() < minSecondsTime) {
            rounds = (optSecondsTime/time.asSeconds())*double(rounds);
            time = benchmark(*i, rounds);
        }
        reporter.report(**i,rounds,time);
    }
}


int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
    
    PerfTestRegistry& registry = PerfTestRegistry::instance();
    registry.registerPerfTest(new RegisterAddPerfTest());
    registry.registerPerfTest(new VolatileAddPerfTest());
    registry.registerPerfTest(new VirtualCallPerfTest());
    registry.registerPerfTest(new MallocFreeCallPerfTest<10>());
    registry.registerPerfTest(new MallocFreeCallPerfTest<100>());
    registry.registerPerfTest(new MallocFreeCallPerfTest<1024>());
    registry.registerPerfTest(new LotMallocFreeCallPerfTest<10,1024>());
    registry.registerPerfTest(new LotMallocFreeCallPerfTest<100,1024>());
    registry.registerPerfTest(new LotMallocFreeCallPerfTest<1024,1024>());
    registry.registerPerfTest(new MemCpyPerfTest<10>());
    registry.registerPerfTest(new MemCpyPerfTest<100>());
    registry.registerPerfTest(new MemCpyPerfTest<1024>());
    
    Reporter reporter;
    benchmarkAll(registry.get(),reporter);
    
    [pool drain];
    return 0;
}
