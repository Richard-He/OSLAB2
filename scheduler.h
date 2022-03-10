#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "event.h"
#include <string>
#include <queue>
#include <stack>
#include <bitset>

#define MAX_PRIO 3000
using namespace std;

class Scheduler{
public:
    int quantum, maxPrio;
    virtual void add_process(Process *p) = 0 ;
    virtual Process* get_next_process() = 0;
    virtual bool test_preempt(Process *p, int curtime) = 0;
    Scheduler(int quantum, int maxPrio): quantum(quantum), maxPrio(maxPrio){}
};


class FCFS : public Scheduler{
    queue<Process*> runQueue;
public:
    FCFS(int quantum, int maxPrio): Scheduler(quantum, maxPrio){}

    void add_process(Process* process) override;

    bool test_preempt(Process *p, int curtime) override;

    Process* get_next_process() override;
};


class LCFS : public Scheduler{
    stack<Process*> runStack;
public:
    LCFS(int quantum, int maxPrio): Scheduler(quantum, maxPrio){}

    void add_process(Process* process) override;

    bool test_preempt(Process *p, int curtime) override;

    Process* get_next_process() override;
};


class RR: public Scheduler{
    queue<Process*> runQueue;
public:
    RR(int quantum, int maxPrio): Scheduler(quantum, maxPrio){}

    void add_process(Process* process) override;

    bool test_preempt(Process *p, int curtime) override;

    Process* get_next_process() override;
};


class SRTF: public Scheduler{
    priority_queue<Process*, vector<Process*>, CompareProcess> runQueue;
public:
    SRTF(int quantum, int maxPrio): Scheduler(quantum, maxPrio){}

    void add_process(Process* process) override;

    bool test_preempt(Process *p, int curtime) override;

    Process* get_next_process() override;
};

class PRIO: public Scheduler{
public:
    vector<queue<Process*> > runQueue;
    vector<queue<Process*> > expiredQueue;
    bitset<MAX_PRIO> runmap;
    bitset<MAX_PRIO> expiredmap;
    PRIO(int quantum, int maxPrio): Scheduler(quantum, maxPrio){
        runQueue = vector<queue<Process*> >(maxPrio, queue<Process*>());
        expiredQueue = vector<queue<Process*> >(maxPrio, queue<Process*>());
    }

    bool isrunQueueempty();

    bool isexpiredQueueempty();

    void add_process(Process* process) override;

    bool test_preempt(Process *p, int curtime) override;

    Process* get_next_process() override;
};

class PREPRIO: public PRIO{
    int lastswitch = 0;
public:
    Process* get_next_process() override;

    PREPRIO(int quantum, int maxPrio): PRIO(quantum, maxPrio){}

    bool test_preempt(Process *p, int curtime) override;

};
#endif