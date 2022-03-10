#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <getopt.h>
#include "DES.h"
#include "event.h"
#include "Random.h"
#include "scheduler.h"
#define DEFAULT_QUANTUM 10000
#define DEFAULT_MAXPRIO 4
//#define DEBUG
using namespace std;

map <char, string> dict{
        {'F', "FCFS"},
        {'L', "LCFS"},
        {'S', "SRTF"},
        {'R', "RR"},
        {'E', "PREPRIO"},
        {'P', "PRIO"}
};
void PrintResult(const string& sname, int quantum, const vector<Process*>& processes, int IOBusyTime){
    cout<<sname;
    if(sname=="RR" || sname=="PRIO" || sname=="PREPRIO") {
        cout<<" "<<quantum;
    }
    cout<<endl;
    double cpuUtil=0, ioUtil=0, avgTurna=0, avgCPUwait=0, throughput=0;
    int lasteventfinish=0, CPUBusyTime=0;
    int num_process = processes.size();
    for(auto proc: processes){
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
               proc->pid,
               proc->ArriveTime,
               proc->TotalTime,
               proc->CpuBurst,
               proc->IoBurst,
               proc->staticPrio,
               proc->FinishTime,
               proc->FinishTime - proc->ArriveTime,
               proc->IoTime,
               proc->CpuWait);
        lasteventfinish = max(lasteventfinish, proc->FinishTime);
        avgCPUwait += proc->CpuWait;
        avgTurna += proc->FinishTime-proc->ArriveTime;
        CPUBusyTime += proc->TotalTime;
    }
    avgCPUwait /= num_process;
    avgTurna /= num_process;
    throughput = 100 * (num_process/(double)lasteventfinish);
    cpuUtil = 100 * (CPUBusyTime/(double)lasteventfinish);
    ioUtil = 100 * (IOBusyTime/(double)lasteventfinish);

    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           lasteventfinish,
           cpuUtil,
           ioUtil,
           avgTurna,
           avgCPUwait,
           throughput);
}

int get_quantum(string s){// Used to get the quantum number
    if(s.size() == 1)
        return DEFAULT_QUANTUM;
    else if(s.find(":")==string::npos)
        return stoi(s.substr(1,s.find(":")-1));
    else
        return stoi(s.substr(1) );
}

int get_maxprio(string s){
    size_t pos =s.find(":");
    if(pos==string::npos||pos == string::npos-1)
        return DEFAULT_MAXPRIO;
    else {
        string str = s.substr(pos + 1);
        return stoi(str);
    }
}

Scheduler* get_scheduler(char sched, int quantum, int maxPrio){
    switch(sched){
        case 'F':
            return new FCFS(DEFAULT_QUANTUM, DEFAULT_MAXPRIO);
        case 'L':
            return new LCFS(DEFAULT_QUANTUM, DEFAULT_MAXPRIO);
        case 'S':
            return new SRTF(DEFAULT_QUANTUM, maxPrio);
        case 'R':
            return new RR(quantum, maxPrio);
        case 'P':
            return new PRIO(quantum, maxPrio);
        case 'E':
            return new PREPRIO(quantum, maxPrio);
    }
}

vector<Process*> get_processes(string &path, int maxPrio, RandomGen* RNG){
    vector<Process*> processes;
    ifstream infile(path);
    int pid =0;
    string line;
    while(getline(infile, line)){
        vector<string> Params;
        istringstream iss(line);
        string token;
        while(getline(iss, token, ' ')){
            if(!token.empty()){
                Params.push_back(token);
            }
        }
        int staticPrio = RNG->random(maxPrio);
        int dynamicPrio = staticPrio -1;
        processes.push_back(new Process(pid, stoi(Params[0]), stoi(Params[1]), stoi(Params[2]),
                                        stoi(Params[3]), staticPrio, dynamicPrio));
        pid++;
    }
    return processes;
}

void print_process(vector<Process*> processes){
    double cpuUtil=0, ioUtil=0, avgTurna=0, avgCPUwait=0;
    int lasteventfinish=0, CPUBusyTime=0;
    for(auto proc: processes){
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
               proc->pid,
               proc->ArriveTime,
               proc->TotalTime,
               proc->CpuBurst,
               proc->IoBurst,
               proc->staticPrio,
               proc->FinishTime,
               proc->FinishTime - proc->ArriveTime,
               proc->IoTime,
               proc->CpuWait);
        lasteventfinish = max(lasteventfinish, proc->FinishTime);
        avgCPUwait += proc->CpuWait;
        avgTurna += proc->FinishTime-proc->ArriveTime;
        CPUBusyTime += proc->TotalTime;
    }
}


int main(int argc, char* argv[]) {
    bool V = false, T= false, E= false, P=false, S=false;
    string s;
    int c;

    while((c = getopt(argc, argv,"s:tepv"))!=-1) {
        switch (c) {
            case 'v':
                V = true;
                break;
            case 's':
                s = optarg;
                break;
            case 't':
                T=true;
                break;
            case 'p':
                P=true;
                break;
            case 'e':
                E=true;
        }
    }
//#ifdef DEBUG
//    cout<<"s"<<s<<endl;
//#endif
    string PROCESS_PATH = argv[optind++];
    string RANDOM_PATH = argv[optind++];
    RandomGen *RNG =new RandomGen(RANDOM_PATH);
//#ifdef DEBUG
//    cout<<"Ppath"<<PROCESS_PATH<<endl;
//    cout<<"Rpath"<<RANDOM_PATH<<endl;
//#endif

    int quantum = get_quantum(s);
    int maxPrio = get_maxprio(s);

    Scheduler * scheduler= get_scheduler(s[0],quantum, maxPrio);
    DES* des = new DES(scheduler, RNG, quantum);
//#ifdef DEBUG
//    cout<<quantum<<endl<<maxPrio<<endl;
//#endif
    vector<Process*> processes= get_processes(PROCESS_PATH, maxPrio, RNG);
#ifdef DEBUG
    print_process(processes);
#endif
    for(auto process:processes){
        des->put_event(new Event(process, process->ArriveTime, CREATED, TRANS_TO_READY, 0));
    }
//#ifdef DEBUG
//    cout<<"successful"<<endl;
//#endif
    des->Simulation();
    int Iotime = des->get_IO_time();
    #ifdef DEBUG
    cout<<"IoTime"<<Iotime<<endl;
#endif
    PrintResult(dict[s[0]], quantum, processes, Iotime);
    return 0;
}
