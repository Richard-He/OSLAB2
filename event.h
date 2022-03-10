#ifndef EVENT_H
#define EVENT_H
enum TS {TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT, TRANS_TO_EXIT};
enum PS {CREATED, READY, RUNNING, BLOCKED};

class Process{
public:
    int ArriveTime, TotalTime, CpuBurst, IoBurst;
    int TimeRemain, BurstRemain, pid, staticPrio, dynamicPrio;
    int FinishTime, IoTime, CpuWait, lastStateTimestamp;

    Process(int pid, int ArriveTime, int TotalTime, int CpuBurst, int IoBurst, int staticPrio, int dynamicPrio) {
        this->pid = pid;
        this->ArriveTime = ArriveTime;
        this->TotalTime = TotalTime;
        this->CpuBurst = CpuBurst;
        this->IoBurst = IoBurst;
        this->staticPrio = staticPrio;
        this->dynamicPrio = dynamicPrio;
        CpuWait = 0;
        FinishTime = 0;
        IoTime = 0;
        lastStateTimestamp = ArriveTime;
        TimeRemain = TotalTime;
        BurstRemain = 0;
    }
};

class Event{
public:
    Process *process;
    int timestamp, event_gen_time;
    PS ps;
    TS ts;

    Event(Process *process, int timestamp, PS ps, TS ts, int event_gen_time){
        this->process =process;
        this->timestamp =timestamp;
        this->ps = ps;
        this->ts =ts;
        this->event_gen_time = event_gen_time;
    }
};

class CompareProcess{
public:
    bool operator() (Process *a, Process *b) const
    {
        if(a->TimeRemain == b->TimeRemain)
            return a->pid > b->pid;
        else
            return a->TimeRemain > b->TimeRemain;
    }
};

class CompareEvent{
public:
    bool operator() (Event* a, Event *b) const
    {
        if (a->timestamp == b->timestamp)
            if (a->event_gen_time == b->event_gen_time)
                return  a->process->pid > b->process->pid;
            else
                return a->event_gen_time > b->event_gen_time;
        else
            return a->timestamp > b->timestamp;
    }
};
#endif