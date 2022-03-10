#ifndef DES_H
#define DES_H
#include <queue>
#include <map>
#include <stack>
#include <iostream>
#include <algorithm>
#include "scheduler.h"
#include "Random.h"
using namespace std;

template<typename T> class new_priority_queue : public std::priority_queue<T, std::vector<T>, CompareEvent>
{
public:

bool remove(const T& value) {
    auto it = std::find(this->c.begin(), this->c.end(), value);
    if (it != this->c.end()) {
        this->c.erase(it);
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
        return true;
    }
    else {
        return false;
    }
}
};

class DES{
    new_priority_queue <Event* > EventQueue;
    int quantum;
    int CURRENT_TIME;
    stack<Process*> Iostack;
    bool CALL_SCHEDULER;
    Scheduler* scheduler;
    RandomGen* RNG;
    Process* CURRENT_RUNNING_PROCESS;
    map<int, Event*> pidtoevent;
    vector<pair<int, int> >IO_period;
public:
    void Simulation();
    void toReady(Process* process);
    void toPreempt(Process* process);
    void toBlock(Process* process);
    void toRun(Process* process);
    void toExit(Process* process);
    DES(Scheduler* scheduler, RandomGen* RNG, int quantum):
            scheduler(scheduler), RNG(RNG), quantum(quantum), CURRENT_TIME(0),
            CURRENT_RUNNING_PROCESS(nullptr) {}
    Event* get_event();
    int get_next_event_time();
    int get_IO_time();
    void put_event(Event* event);// Put event into the prioque
    void rm_event(Event* event);// Remove event from the prioque
};
#endif