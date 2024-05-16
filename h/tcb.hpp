#ifndef _THREAD_HPP
#define _THREAD_HPP

#include "../lib/hw.h"
#include "../lib/mem.h"
#include "scheduler.hpp"

class TCB {
public:
    using Body = void (*)(void*);
    static TCB *createThread(Body body, void* arg, uint8* stack_space);
    void* operator new(size_t size) {
        return __mem_alloc(size);
    }
    void operator delete(void* ptr) {
        __mem_free(ptr);
    }
    static void dispatch();
    static void exit();

    inline bool isFinished() const {return finished;}
    inline void setFinished(bool finished) { TCB::finished = finished;}

    static TCB* running;
private:
    friend class Riscv;
    friend class Scheduler;
    TCB(Body body, void* arg, uint8* stack_space) :
        body(body),
        stack(stack_space),
        arg(arg),
        context({
            (uint64)&wrapper,
            (uint64)(body?&stack[DEFAULT_STACK_SIZE]:0)
        }),
        finished(false),
        next(nullptr) {
        if(body) {
            Scheduler::put(this);
        }
    }
    struct Context {
        uint64 ra;
        uint64 sp;
    };
    Body body;
    uint8* stack;
    void* arg;
    Context context;
    bool finished;
    TCB* next;
    static void contextSwitch(Context* oldRunning, Context* newRunning);
    static void wrapper();
};


#endif //_THREAD_HPP
