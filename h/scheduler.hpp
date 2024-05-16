#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

class TCB;

class Scheduler
{
private:
    static TCB* head;

public:
    static TCB *get();

    static void put(TCB *thread);

};

#endif //_SCHEDULER_HPP
