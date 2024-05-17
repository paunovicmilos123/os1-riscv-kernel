#ifndef _KERNELSEMAPHORE_HPP
#define _KERNELSEMAPHORE_HPP

#include "../lib/mem.h"
#include "../lib/hw.h"
#include "../h/tcb.hpp"

enum sem_const {
    SEM_DEAD = -2,
    SEM_TIMEOUT = -1
};

/*
 * Implementacija semafora slicna onoj u skolskom jezgru
 */
class kSemaphore {
public:
    kSemaphore(unsigned init) : value((long)init), blocked(nullptr), open(true) {

    }
    void* operator new(size_t size) {
        return __mem_alloc(size);
    }

    int wait();
    int trywait();
    int signal();
    int close();

protected:
    void block();
    void unblock();
private:
    long value;
    TCB* blocked;
    volatile bool open;
};


#endif //_KERNELSEMAPHORE_HPP
