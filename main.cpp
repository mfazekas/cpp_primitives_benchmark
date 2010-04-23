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

#include <string>
#include <iostream>
#include <sstream>

#include "IPerfTest.h"
#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

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

class TextReporter : public IReporter {
public:
    virtual void report(IPerfTest& perfTest,int rounds,const Time& time) {
        std::cout << "name:" << perfTest.name() << ": " << double(rounds)/time.asSeconds() << " " << perfTest.unit() << "/seconds" << std::endl;
    }
};

#include <map>

using std::string;

class XMLReporter : public IReporter {
public:
    XMLReporter(std::ostream& os) : os(os) { start(); }
    ~XMLReporter() { stop(); }
    
    string xmlquote(const std::string& input) {
        string toquote = "<>";
        std::map<char,string> replace_table;
        replace_table['<'] = "&lt;";
        replace_table['>'] = "&gt;";
        string result = input;
        string::size_type begIndex = -1;
        while ((begIndex = result.find_first_of(toquote,begIndex+1)) != std::string::npos) {
            result.replace(begIndex,1,replace_table[result[begIndex]]);
        }
        return result;
    }
    void start() {
        os << "<reports>" << std::endl;
    }
    void stop() {
        os << "</reports>" << std::endl;
    }
    virtual void report(IPerfTest& perfTest,int rounds,const Time& time) {
        os << "<report>" << std::endl;
        os << "<name>" << xmlquote(perfTest.name()) << "</name>" << std::endl;
        os << "<unit>" << std::endl;
        os << "<name>" << perfTest.unit() << "</name>" << std::endl;
        os << "<amount>" << rounds << "</amount>" << std::endl;
        os << "<persec>" << double(rounds)/time.asSeconds() << "</persec>" << std::endl;
        os << "</unit>" << std::endl;
        os << "<rounds>" << rounds << "</rounds>" << std::endl;
        os << "<time>" << time.asSeconds() << "</time>" << std::endl;
        os << "</report>" << std::endl;
    }
private:
    std::ostream& os;
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
    PerfTestRegistry& registry = PerfTestRegistry::instance();
    //Reporter reporter;
    XMLReporter reporter(std::cout);
    benchmarkAll(registry.get(),reporter);
    return 0;
}
