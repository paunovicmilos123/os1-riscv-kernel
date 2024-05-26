#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

TCB* Scheduler::head = nullptr;
TCB* Scheduler::kernelThreads = nullptr;
TCB* Scheduler::sleeping = nullptr;

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

// lista uspavanih implementirana po profesorovom predlogu iz postavke projekta
void Scheduler::addSleeping(time_t t) {
    time_t relativeTime = t;
    TCB::running->setReady(false);

    TCB* prev = nullptr;
    TCB* next = sleeping;
    while(next) {
        if(relativeTime >= next->sleepRemaining) {
            relativeTime -= next->sleepRemaining;
            prev = next;
            next = next->next;
        } else {
            next->sleepRemaining -= relativeTime;
            break;
        }
    }

    TCB::running->sleepRemaining = relativeTime;

    if(prev) prev->next = TCB::running;
    else sleeping = TCB::running;
    TCB::running->next = next;
}

void Scheduler::updateSleeping() {
    if(sleeping)
        sleeping->sleepRemaining--;
    while(sleeping && sleeping->sleepRemaining==0) {
        TCB* woken = sleeping;
        woken->setReady(true);
        sleeping = sleeping->next;
        put(woken);
    }
}