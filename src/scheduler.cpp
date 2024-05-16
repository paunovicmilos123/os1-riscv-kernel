#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

TCB* Scheduler::head = nullptr;

TCB *Scheduler::get()
{
    TCB* ret = nullptr;
    if(head) {
        ret = head;
        head = head->next;
        ret->next = nullptr;
    }

    return ret;
}

void Scheduler::put(TCB *thread)
{
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