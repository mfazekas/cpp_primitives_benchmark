 
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

