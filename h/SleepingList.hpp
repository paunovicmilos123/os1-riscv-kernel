#ifndef _SLEEPINGLIST_HPP
#define _SLEEPINGLIST_HPP

#include "../lib/hw.h"
#include "../lib/mem.h"

class TCB;

class SleepingList {
public:
    SleepingList() : sleeping(nullptr) {

    }
    void* operator new(size_t size) {
        return __mem_alloc(size);
    }
    void operator delete(void* ptr) noexcept {
        ((SleepingList*)ptr)->wakeAll();
        __mem_free(ptr);
    }
    TCB* sleeping;

    void addSleeping(time_t);
    void updateSleeping();
    void wakeOne();
    void wakeAll();
};


#endif