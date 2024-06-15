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

int SleepingList::updateSleeping() {
    int numberOfWoken = 0;
    if(sleeping)
        sleeping->sleepRemaining--;
    while(sleeping && sleeping->sleepRemaining==0) {
        wakeOne();
        numberOfWoken++;
    }
    return numberOfWoken;
}

void SleepingList::wakeOne(bool priority) {
    if (!sleeping) return;
    if(priority) {
        TCB *t,*min,*prev=nullptr;
        min = t = sleeping;
        while(t->next) {
            if(min->id > t->next->id) {
                prev = t;
                min = t->next;
            }
            t = t->next;
        }
        if(prev) {
            prev->next = min->next;
            if(prev->next) prev->next->sleepRemaining += min->sleepRemaining;
        } else {
            sleeping = min->next;
            if(sleeping) sleeping->sleepRemaining += min->sleepRemaining;
        }
        min->setReady(true);
        Scheduler::put(min);
        return;
    }
    TCB *woken = sleeping;
    woken->setReady(true);
    sleeping = sleeping->next;
    if(sleeping) sleeping->sleepRemaining += woken->sleepRemaining;
    Scheduler::put(woken);
}

void SleepingList::wakeAll() {
    while(sleeping) {
        wakeOne();
    }
}

uint64 SleepingList::getMinPriority() {
    if(!sleeping) return ~0UL;
    TCB* t = sleeping;
    uint64 min = sleeping->id;
    while(t) {
        if(min > t->id) min = t->id;
        t = t->next;
    }
    return min;
}