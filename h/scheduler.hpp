#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

#include "../lib/hw.h"
#include "SleepingList.hpp"

class TCB;

class Scheduler
{
private:
    static TCB* head;
    static TCB* kernelThreads;
    static SleepingList sleepingList;

public:
    static TCB *get(bool getKernelThread=false);
    static void put(TCB *thread, bool toFront=false);

    static void addSleeping(time_t);
    static void updateSleeping();

    static void killAll();
};

#endif //_SCHEDULER_HPP
