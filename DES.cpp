#include "DES.h"
#include "Random.h"
#include <algorithm>
//#define DEBUG
using namespace std;

map<PS, string>PSdic{
        {CREATED, "CREATED"},
        {READY, "READY"},
        {RUNNING, "RUNNING"},
        {BLOCKED, "BLOCKED"}
};

map<TS, string>TSdic{
        {TRANS_TO_READY, "TRANS_TO_READY"},
        {TRANS_TO_RUN,"TRANS_TO_RUN"},
        {TRANS_TO_BLOCK,"TRANS_TO_BLOCK"},
        {TRANS_TO_PREEMPT,"TRANS_TO_PREEMPT"},
        {TRANS_TO_EXIT,"TRANS_TO_EXIT"}
};

void print_event(Event* evt){
    cout<<"PID\t"<<evt->process->pid<<"\tTimeRemain\t"<<evt->process->TimeRemain<<endl;
    cout<<"CPUBurstRemain\t"<<evt->process->BurstRemain<<"\tIoTime\t"<<evt->process->IoTime<<endl;
    cout<<"CPUWaitTime\t"<<evt->process->CpuWait<<"\tlastStateTimestamp\t"<<evt->process->lastStateTimestamp<<endl;
    cout<<"timestamp\t"<<evt->timestamp<<"\tGen_time\t"<<evt->event_gen_time<<endl;
    cout<<"PS\t"<<PSdic[evt->ps]<<"\tTS\t"<<TSdic[evt->ts]<<endl<<endl<<endl<<endl;
}

void DES::Simulation() {
    Event* evt;
    while((evt = get_event())){
#ifdef DEBUG
        print_event(evt);
#endif
        Process *proc = evt->process;
        CURRENT_TIME = evt->timestamp;
        TS transition = evt->ts;
        PS evt_pre_state = evt->ps;
        int timeInPrevState = CURRENT_TIME - proc->lastStateTimestamp;
        delete evt;
        evt = nullptr;
        switch(transition){
            case TRANS_TO_READY:
                if(evt_pre_state == BLOCKED) {
                    IO_period.push_back(make_pair(proc->lastStateTimestamp,CURRENT_TIME));
                    proc->IoTime += timeInPrevState;
                    proc -> dynamicPrio = proc->staticPrio -1;
                }
                toReady(proc);
//                if(CURRENT_RUNNING_PROCESS && scheduler->test_preempt(CURRENT_RUNNING_PROCESS, CURRENT_TIME)){
                if(CURRENT_RUNNING_PROCESS && scheduler->test_preempt(CURRENT_RUNNING_PROCESS, CURRENT_TIME)
                   && pidtoevent[CURRENT_RUNNING_PROCESS->pid]->timestamp > CURRENT_TIME){
                    rm_event(pidtoevent[CURRENT_RUNNING_PROCESS->pid]);
                    Event* newevt = new Event(CURRENT_RUNNING_PROCESS, CURRENT_TIME, RUNNING, TRANS_TO_PREEMPT, CURRENT_TIME);
                    put_event(newevt);
                    pidtoevent[CURRENT_RUNNING_PROCESS->pid] = newevt;
                }
                CALL_SCHEDULER = true;
                break;
            case TRANS_TO_RUN:
                proc->CpuWait += timeInPrevState;
                toRun(proc);
                break;
            case TRANS_TO_BLOCK:
                toBlock(proc);
                CURRENT_RUNNING_PROCESS = nullptr;
                CALL_SCHEDULER = true;
                break;
            case TRANS_TO_PREEMPT:
                toPreempt(proc);
                CURRENT_RUNNING_PROCESS = nullptr;
                CALL_SCHEDULER = true;
                break;
            case TRANS_TO_EXIT:
                toExit(proc);
                CALL_SCHEDULER = true;
                break;
        }
        proc->lastStateTimestamp = CURRENT_TIME;
        if(CALL_SCHEDULER){
            if(get_next_event_time()==CURRENT_TIME)
                continue;
            CALL_SCHEDULER=false;
            if(CURRENT_RUNNING_PROCESS== nullptr){
                CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
                if(CURRENT_RUNNING_PROCESS== nullptr)
                    continue;
                put_event(new Event(CURRENT_RUNNING_PROCESS, CURRENT_TIME, READY,
                                    TRANS_TO_RUN, CURRENT_TIME));
            }
        }
    }
}

void DES::toReady(Process *process) {
    process->lastStateTimestamp = CURRENT_TIME;
    scheduler->add_process(process);
}

void DES::toRun(Process *process) {
    if(process->BurstRemain ==0){
        process->BurstRemain = RNG->random(process->CpuBurst);
    }
    process->BurstRemain = min(process->BurstRemain, process->TimeRemain);

    if(process->BurstRemain>quantum){
        Event *event = new Event(process, CURRENT_TIME+quantum, RUNNING,
                                 TRANS_TO_PREEMPT, CURRENT_TIME);
        put_event(event);
        pidtoevent[process->pid] = event;
        return;
    }
    if(process->TimeRemain>process->BurstRemain){
        Event *event = new Event(process, CURRENT_TIME + process->BurstRemain, RUNNING,
                                 TRANS_TO_BLOCK, CURRENT_TIME);
        put_event(event);
        pidtoevent[process->pid] = event;
    }
    else{
        Event *event = new Event(process, CURRENT_TIME+process->BurstRemain, RUNNING,
                                 TRANS_TO_EXIT, CURRENT_TIME);
        put_event(event);
        pidtoevent[process->pid] = event;
    }
}

void DES::toExit(Process *process){
    process->FinishTime = CURRENT_TIME;
    CURRENT_RUNNING_PROCESS = nullptr;
}

int DES::get_next_event_time() {
    if(EventQueue.empty())
        return -1;
    else
        return EventQueue.top()->timestamp;
}

void DES::rm_event(Event *event){
    EventQueue.remove(event);
    delete event;
}

void DES::put_event(Event *event){
    EventQueue.push(event);
}

Event* DES:: get_event(){
    if(EventQueue.empty())
        return nullptr;
    Event *event =EventQueue.top();
    EventQueue.pop();
    return event;
}

void DES::toBlock(Process *process) {
    int runtime = CURRENT_TIME - process->lastStateTimestamp;
    process->TimeRemain -= runtime;
    process->BurstRemain -= runtime;
    if(process->TimeRemain >0){
        int ioBurst = RNG->random(process->IoBurst);
        put_event(new Event(process, CURRENT_TIME+ioBurst,
                            BLOCKED, TRANS_TO_READY, CURRENT_TIME));
    }
}

void DES::toPreempt(Process *process) {
    int runtime = CURRENT_TIME-process->lastStateTimestamp;
    process->TimeRemain -=runtime;
    process->BurstRemain -=runtime;
    process->dynamicPrio -=1;
    put_event(new Event(process, CURRENT_TIME,
                        RUNNING, TRANS_TO_READY,CURRENT_TIME));
}

bool compareInterval(pair<int,int> i1, pair<int, int> i2){
    return (i1.first < i2.first);
}

int DES::get_IO_time(){
    if(IO_period.empty())
        return 0;
    int overlap = 0, size = IO_period.size();
    sort(IO_period.begin(), IO_period.end());
    int beg = IO_period[0].first, end = IO_period[0].second;
    for(int i=1; i<size; i++){
        if(IO_period[i].first>end){
            overlap += end-beg;
            beg = IO_period[i].first;
        }
        if(IO_period[i].second >end){
            end = IO_period[i].second;
        }
    }
    overlap += end - beg;
    return overlap;
}