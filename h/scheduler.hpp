#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

#include "../lib/hw.h"

class TCB;

class Scheduler
{
private:
    static TCB* head;
    static TCB* kernelThreads;
    static TCB* sleeping;

public:
    static TCB *get(bool getKernelThread=false);
    static void put(TCB *thread, bool toFront=false);

    static void addSleeping(time_t);
    static void updateSleeping();

};

#endif //_SCHEDULER_HPP
