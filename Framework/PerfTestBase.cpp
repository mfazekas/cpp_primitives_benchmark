/*
 *  PerfTestBase.cpp
 *  PrimitveOperationsPerfTest
 *
 *  Created by Miklós Fazekas on 3/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "PerfTestBase.h"

void PerfTestBase::setup(int& rounds_,int fourtytwo_,int random_) {
}

void PerfTestBase::teardown(int rounds_,int fourtytwo_,int random_) {
}

std::string PerfTestBase::unit() const { 
    return "operations"; 
}

int PerfTestBase::defaultRounds() const { 
    return 100000; 
}
