#include "scheduler.h"
#include <algorithm>
using namespace std;

void FCFS::add_process(Process *process) {
    runQueue.push(process);
}

Process* FCFS::get_next_process() {
    if(runQueue.empty())
        return nullptr;
    Process *process = runQueue.front();
    runQueue.pop();
    return process;
}

bool FCFS::test_preempt(Process *p, int curtime) {
    return false;
}

void LCFS::add_process(Process *process) {
    runStack.push(process);
}

Process* LCFS::get_next_process() {
    if(runStack.empty())
        return nullptr;
    Process* process = runStack.top();
    runStack.pop();
    return process;
}

bool LCFS::test_preempt(Process *process, int curtime) {
    return false;
}

void RR::add_process(Process *process) {
    runQueue.push(process);
}

Process* RR::get_next_process() {
    if(runQueue.empty())
        return nullptr;
    Process *process = runQueue.front();
    runQueue.pop();
    return process;
}

bool RR::test_preempt(Process *process, int curtime) {
    return false;
}

void SRTF::add_process(Process *process) {
    runQueue.push(process);
}

Process* SRTF::get_next_process() {
    if(runQueue.empty())
        return nullptr;
    Process* process = runQueue.top();
    runQueue.pop();
    return process;
}

bool SRTF::test_preempt(Process *process, int curtime) {
    return false;
}

void PRIO::add_process(Process *process) {
    if(process->dynamicPrio==-1){
        process->dynamicPrio = process->staticPrio-1;
        expiredQueue[process->dynamicPrio].push(process);
        if(expiredmap[process->dynamicPrio]==0)
            expiredmap[process->dynamicPrio] = 1;
    }
    else{
        runQueue[process->dynamicPrio].push(process);
        if(runmap[process->dynamicPrio]==0)
            runmap[process->dynamicPrio] =1;
    }
}

Process* PRIO::get_next_process() {
    if(isrunQueueempty()){
        if(isexpiredQueueempty())
            return nullptr;
        else
        {
            swap(runQueue, expiredQueue);
            swap(runmap, expiredmap);
        }
    }

    int endd = maxPrio - 1;
    while(!runmap[endd]) {
        endd--;
    }
    Process* process = runQueue[endd].front();
    runQueue[endd].pop();
    if(runQueue[endd].empty())
        runmap[endd]=0;
    return process;
}

bool PRIO::isrunQueueempty(){
    return !runmap.any();
}

bool PRIO::isexpiredQueueempty() {
    return !expiredmap.any();
}

bool PRIO::test_preempt(Process *process, int curtime) {
    return false;
}

bool PREPRIO::test_preempt(Process *process, int curtime) {
//    if(curtime - process->lastStateTimestamp < quantum)
//        return false;
    int endd = maxPrio - 1;
    while(!runmap[endd]) {
        endd--;
    }
    if(process->dynamicPrio < endd)
        return true;
    else
        return false;
}

Process* PREPRIO::get_next_process() {
    if(isrunQueueempty()){
        if(isexpiredQueueempty())
            return nullptr;
        else
        {
            swap(runQueue, expiredQueue);
            swap(runmap, expiredmap);
        }
    }

    int endd = maxPrio - 1;
    while(!runmap[endd]) {
        endd--;
    }
    Process* process = runQueue[endd].front();
    runQueue[endd].pop();
    if(runQueue[endd].empty())
        runmap[endd]=0;
    return process;
}
//PRIO::PRIO(int quantum, int maxPrio):
//        BaseScheduler(quantum, maxPrio){
//    runQueue = vector<queue<Process*> >(maxPrio, queue<Process*>());
//    expiredQueue = vector<queue<Process*> >(maxPrio, queue<Process*>());
//}
