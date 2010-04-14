/*
Copyright (c) 2007-, Miklós Fazekas
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



PERFTEST_REGISTER(WriteSysCallPerfTest,new WriteSysCallPerfTest());
PERFTEST_REGISTER(GetUIDSysCallPerfTest,new GetUIDSysCallPerfTest());
