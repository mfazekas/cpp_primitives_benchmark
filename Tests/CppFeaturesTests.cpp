 
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

class DynamicCastPerfTest : public PerfTestBase {
public:
    class Base {
    public:
        virtual ~Base() {};
        
    };
    class DerivedA : public Base {
    public:
        virtual void dosomething() {}
    };
    class DerivedB : public Base {
    };
    
    virtual void setup(int& rounds_,int fourtytwo_,int random_) {
        b = new DerivedA();
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        for (int i = 0; i < rounds_; ++i) {
            DerivedA* a = dynamic_cast<DerivedA*>(b);
            if (fourtytwo_ != 42) { a->dosomething(); }
        }
        return 0;
    }
    std::string name() const {
        return "c++dynamiccastfrombasetosub";
    }    
    
    Base* b;
    
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

PERFTEST_AUTOREGISTER(VirtualCallPerfTest,new VirtualCallPerfTest());
PERFTEST_AUTOREGISTER(DynamicCastPerfTest,new DynamicCastPerfTest());



