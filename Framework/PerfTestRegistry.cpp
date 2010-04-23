
#include "PerfTestRegistry.h"

PerfTestRegistry& PerfTestRegistry::instance() {
    static PerfTestRegistry result;
    return result;
}

void PerfTestRegistry::registerPerfTest(IPerfTest* perfTest) {
    perfTests.push_back(perfTest);
}

std::vector<IPerfTest*> PerfTestRegistry::get() const {
    return perfTests;
}
