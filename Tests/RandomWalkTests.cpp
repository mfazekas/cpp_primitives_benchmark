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

#include "PerfTestBase.h"
#include "PerfTestRegistry.h"

#include <sstream>
#include <deque>

class RandomWalkTests : public PerfTestBase {
public:
    RandomWalkTests(int wssize) : wssize((wssize/sizeof(Node))*sizeof(Node)) {
        int count = wssize/sizeof(Node);
        nodes = new Node[count];
        std::vector<std::pair<int,Node*> > items;
        srand(1);
        for (int i = 0; i < count; ++i) {
            items.push_back(std::make_pair(rand(),nodes+i));
        }
        std::sort(items.begin(),items.end());
        for (int i = 0; i < count; ++i) {
            items[i].second->next = items[(i+1)%count].second;
        }

        warmup();
    }
    void warmup() {
        Node* act = nodes;
        for (int i =0 ; i < wssize/sizeof(Node); ++i) {
            act = act->next;
        }
    }
    ~RandomWalkTests() {
        delete[] nodes;
    }
    virtual int perform (int& rounds_,int fourtytwo_,int random_) {
        Node* act = nodes;
        for (int i = 0; i < rounds_; ++i) {
            act = act->next;
        }
        return act-nodes;
    }
    std::string name() const {
        std::ostringstream os;
        os << " random walk test working set size:" << wssize;
        return os.str();
    }
    struct Node  {
        Node* next;
    };
    int wssize;
    Node *nodes;
};

PERFTEST_REGISTER(RandomWalkTests256,new RandomWalkTests(256));
PERFTEST_REGISTER(RandomWalkTests1024,new RandomWalkTests(1024));
PERFTEST_REGISTER(RandomWalkTests16kb,new RandomWalkTests(16*1024));
PERFTEST_REGISTER(RandomWalkTests32kb,new RandomWalkTests(32*1024));
PERFTEST_REGISTER(RandomWalkTests64kb,new RandomWalkTests(64*1024));
PERFTEST_REGISTER(RandomWalkTests256kb,new RandomWalkTests(256*1024));
PERFTEST_REGISTER(RandomWalkTests512kb,new RandomWalkTests(512*1024));
PERFTEST_REGISTER(RandomWalkTests1M,new RandomWalkTests(1024*1024));
PERFTEST_REGISTER(RandomWalkTests2M,new RandomWalkTests(2*1024*1024));
PERFTEST_REGISTER(RandomWalkTests4M,new RandomWalkTests(4*1024*1024));
PERFTEST_REGISTER(RandomWalkTests16M,new RandomWalkTests(16*1024*1024));
