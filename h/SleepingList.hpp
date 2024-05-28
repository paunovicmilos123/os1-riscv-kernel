#ifndef _SLEEPINGLIST_HPP
#define _SLEEPINGLIST_HPP

#include "../lib/hw.h"

class TCB;

class SleepingList {
public:
    SleepingList() : sleeping(nullptr) {

    }
    TCB* sleeping;

    void addSleeping(time_t);
    void updateSleeping();
};


#endif