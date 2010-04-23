
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

PERFTEST_AUTOREGISTER(RandomWalkTests256,new RandomWalkTests(256));
PERFTEST_AUTOREGISTER(RandomWalkTests1024,new RandomWalkTests(1024));
PERFTEST_AUTOREGISTER(RandomWalkTests16kb,new RandomWalkTests(16*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests32kb,new RandomWalkTests(32*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests64kb,new RandomWalkTests(64*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests256kb,new RandomWalkTests(256*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests512kb,new RandomWalkTests(512*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests1M,new RandomWalkTests(1024*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests2M,new RandomWalkTests(2*1024*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests4M,new RandomWalkTests(4*1024*1024));
PERFTEST_AUTOREGISTER(RandomWalkTests16M,new RandomWalkTests(16*1024*1024));
