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
 
class RegisterAddPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        register int result = random_;
        BEGIN_REPEAT(rounds_) {
            result += random_;
        } END_REPEAT;
        return result;
    }
    std::string name() const {
        return "register int::operator+=";
    }
};

class TwoIndependentRegisterAddPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int rounds = rounds_/2;
        rounds_ = rounds*2;
        register int result1 = random_;
        register int result2 = random_+fourtytwo_;
        BEGIN_REPEAT(rounds) {
            result1 += random_;
            result2 -= random_;
        } END_REPEAT;
        return (result1+fourtytwo_)*(result2+fourtytwo_);
    }
    std::string name() const {
        return "2 independent register int::operator+= ";
    }
};

class ThreeIndependentRegisterAddPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int rounds = rounds_/3;
        rounds_ = rounds*3;
        register int result1 = random_;
        register int result2 = random_+fourtytwo_;
        register int result3 = random_-fourtytwo_;
        BEGIN_REPEAT(rounds) {
            result1 += random_;
            result2 -= random_;
            result3 += random_;
        } END_REPEAT;
        return (result1+fourtytwo_)*(result2+fourtytwo_)*(result3-fourtytwo_);
    }
    std::string name() const {
        return "3 independent register int::operator+= ";
    }
};

class IntDividePerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        register int result = random_;
        BEGIN_REPEAT(rounds_) {
            result = result/fourtytwo_;
        } END_REPEAT;
        return result;
    }
    std::string name() const {
        return "int::operator/";
    }
};

class VolatileAddPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int32_t result;
        BEGIN_REPEAT(rounds_) {
            result += random_;
        } END_REPEAT;
        return result;
    }
    std::string name() const {
        return "volatile int::operator+=";
    }
    volatile int result;
};

PERFTEST_AUTOREGISTER(RegisterAddPerfTest,new RegisterAddPerfTest());
PERFTEST_AUTOREGISTER(VolatileAddPerfTest,new VolatileAddPerfTest());
PERFTEST_AUTOREGISTER(ThreeIndependentRegisterAddPerfTest,new ThreeIndependentRegisterAddPerfTest());
PERFTEST_AUTOREGISTER(TwoIndependentRegisterAddPerfTest,new TwoIndependentRegisterAddPerfTest());
PERFTEST_AUTOREGISTER(IntDividePerfTest,new IntDividePerfTest());

