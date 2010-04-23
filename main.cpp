
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
