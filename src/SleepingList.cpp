#include "../h/SleepingList.hpp"

#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"

// lista uspavanih implementirana po profesorovom predlogu iz postavke projekta
void SleepingList::addSleeping(time_t t) {
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

void SleepingList::updateSleeping() {
    if(sleeping)
        sleeping->sleepRemaining--;
    while(sleeping && sleeping->sleepRemaining==0) {
        TCB* woken = sleeping;
        woken->setReady(true);
        sleeping = sleeping->next;
        Scheduler::put(woken);
    }
}