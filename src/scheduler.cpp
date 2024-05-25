#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

TCB* Scheduler::head = nullptr;
TCB* Scheduler::kernelThreads = nullptr;

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