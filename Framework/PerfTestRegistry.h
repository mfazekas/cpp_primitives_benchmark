#ifndef PerfRegistryHeader
#define PerfRegistryHeader
 
#include "IPerfTest.h"
#include <vector>


class PerfTestRegistry {
public:
    static PerfTestRegistry& instance();
    
    void registerPerfTest(IPerfTest* perfTest);
    std::vector<IPerfTest*> get() const;
private:
    std::vector<IPerfTest*> perfTests;
};

#define PERFTEST_AUTOREGISTER(name,new_expression) \
    static struct _RegClass##name { _RegClass##name() { PerfTestRegistry::instance().registerPerfTest(new_expression); }} _reg##name
#define PERFTEST_REGISTER(new_expression) \
    do { PerfTestRegistry::instance().registerPerfTest(new_expression); } while (0)
 
#endif // PerfRegistryHeader

