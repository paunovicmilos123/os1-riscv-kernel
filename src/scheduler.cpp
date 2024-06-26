#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

TCB* Scheduler::head = nullptr;
TCB* Scheduler::kernelThreads = nullptr;
SleepingList Scheduler::sleepingList;

TCB *Scheduler::get(bool getKernelThread)
{
    if(getKernelThread) {
        TCB* ret = nullptr;
        if(kernelThreads) {
            ret = kernelThreads;
            kernelThreads = kernelThreads->next;
            ret->next = nullptr;
        }
        return ret;
    }
    TCB* ret = nullptr;
    if(head) {
        ret = head;
        head = head->next;
        ret->next = nullptr;
    }

    // ako je proslu nit prekinula interna nit kernela, ne obnavlja joj se time slice
    if(!TCB::running->isKernelThread)
        TCB::ticksRemaining = ret->getTimeSlice();

    return ret;
}

void Scheduler::put(TCB *thread, bool toFront)
{
    if(thread->isKernelThread) {
        if(kernelThreads==nullptr) {
            kernelThreads = thread;
            thread->next = nullptr;
            return;
        }
        TCB* last = kernelThreads;
        while(last->next) {
            last = last->next;
        }
        last->next = thread;
        thread->next = nullptr;
        return;
    }

    if(toFront) {
        thread->next = head;
        head = thread;
        return;
    }
    if(head==nullptr) {
        head = thread;
        head->next = nullptr;
        return;
    }
    TCB* last = head;
    while(last->next) {
        last = last->next;
    }
    last->next = thread;
    thread->next = nullptr;
}

void Scheduler::addSleeping(time_t t) {
    sleepingList.addSleeping(t);
}

void Scheduler::updateSleeping() {
    sleepingList.updateSleeping();
}

void Scheduler::killAll() {
    sleepingList.wakeAll();
    TCB* t;
    while(head) {
        t = head;
        head = head->next;
        delete t;
    }
    while(kernelThreads) {
        t = kernelThreads;
        kernelThreads = kernelThreads->next;
        delete t;
    }
}
