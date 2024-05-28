#ifndef _KERNELSEMAPHORE_HPP
#define _KERNELSEMAPHORE_HPP

#include "../lib/mem.h"
#include "../lib/hw.h"
#include "tcb.hpp"
#include "SleepingList.hpp"

enum sem_const {
    SEM_DEAD = -1,
    SEM_TIMEOUT = -2
};

/*
 * Implementacija semafora slicna onoj u skolskom jezgru
 */
class kSemaphore {
public:
    kSemaphore(unsigned init) :
        value((long)init),
        blocked(nullptr),
        sleepingList(),
        open(true) {
        next = head;
        head = this;
    }
    void* operator new(size_t size) {
        return __mem_alloc(size);
    }

    int wait();
    int timedwait(time_t);
    int trywait();
    int signal();
    int close();

    static void updateSleeping();

protected:
    void block();
    bool unblock();
    void timedBlock(time_t);
    void tick();
private:
    long value;
    TCB* blocked;
    SleepingList sleepingList;
    volatile bool open;

    kSemaphore* next;

    static kSemaphore* head;
};


#endif
