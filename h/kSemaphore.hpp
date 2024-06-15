#ifndef _KERNELSEMAPHORE_HPP
#define _KERNELSEMAPHORE_HPP

#include "../lib/hw.h"
#include "../h/kAllocator.hpp"
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
    kSemaphore(unsigned init, bool priority=false) :
        value((long)init),
        blocked(nullptr),
        sleepingList(),
        open(true),
        priority(priority) {
        next = head;
        head = this;
    }
    void* operator new(size_t size) {
        return kAllocator::alloc(size);
    }
    void operator delete(void* ptr) noexcept {
        ((kSemaphore*)ptr)->close();
        kAllocator::free(ptr);
    }

    int wait();
    int timedwait(time_t);
    int trywait();
    int signal();
    int close();

    static void updateSleeping();
    static void deleteAll();

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
    bool priority;

    kSemaphore* next;

    static kSemaphore* head;
};


#endif
