

#include "ErrorCheck.h"

#include "PerfTestBase.h"
#include "PerfTestRegistry.h"
#include <iostream>
#include <sys/errno.h>
#include <fcntl.h>
#include <sstream>

class GetUIDSysCallPerfTest : public PerfTestBase {
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = random_;
        for (int i = 0; i < rounds_; ++i) {
            result += getuid();
        }
        return result;
    }
    std::string name() const {
        return "syscall getuid";
    }
};

class WriteSysCallPerfTest : public PerfTestBase {
    virtual void setup(int& roudnds_,int fourtytwo_,int random_) {
      fd = ::open("/dev/null",O_WRONLY);
      if (fd < 0) {
        std::cerr << "failed to open /dev/null:" << errno << std::endl;
      }
    }
    virtual void teardown(int roudnds_,int fourtytwo_,int random_) {
      ::close(fd);
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        int result = random_;
        for (int i = 0; i < rounds_; ++i) {
            result += ::write(fd,"",0);
        }
        return result;
    }
    std::string name() const {
        return "syscall write 0 bytes to /dev/null";
    }
    int fd;
};

PERFTEST_AUTOREGISTER(WriteSysCallPerfTest,new WriteSysCallPerfTest());
PERFTEST_AUTOREGISTER(GetUIDSysCallPerfTest,new GetUIDSysCallPerfTest());
