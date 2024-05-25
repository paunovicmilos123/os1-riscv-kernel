#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

class TCB;

class Scheduler
{
private:
    static TCB* head;
    static TCB* kernelThreads;

public:
    static TCB *get(bool getKernelThread=false);

    static void put(TCB *thread, bool toFront=false);

};

#endif //_SCHEDULER_HPP
