/*
Copyright (c) 2007-, cpp_primitives_benchmark contributors
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



