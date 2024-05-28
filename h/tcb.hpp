#ifndef _THREAD_HPP
#define _THREAD_HPP

#include "../lib/hw.h"
#include "../lib/mem.h"
#include "scheduler.hpp"
#include "kSemaphore.hpp"

class TCB {
public:
    using Body = void (*)(void*);
    static TCB *createThread(Body body, void* arg, uint8* stack_space, bool isKernelThread=false);
    void* operator new(size_t size) {
        return __mem_alloc(size);
    }
    void operator delete(void* ptr) noexcept {
        __mem_free(ptr);
    }
    static void dispatch(bool dispatchToKernelThread=false);
    static void exit();

    inline bool isFinished() const {return finished;}
    inline void setFinished(bool finished) { TCB::finished = finished;}
    inline time_t getTimeSlice() const { return timeSlice; }
    inline bool isReady() const { return ready; }
    inline void setReady(bool ready) { this->ready = ready; }

    static TCB* running;
    static time_t ticksRemaining;
private:
    friend class Riscv;
    friend class Scheduler;
    friend class kSemaphore;
    friend class SleepingList;
    TCB(Body body, void* arg, uint8* stack_space, bool isKernelThread=false) :
        body(body),
        stack(stack_space),
        arg(arg),
        context({
            (uint64)&wrapper,
            (uint64)(body?&stack[DEFAULT_STACK_SIZE]:0)
        }),
        finished(false),
        next(nullptr),
        isKernelThread(isKernelThread),
        timeSlice(DEFAULT_TIME_SLICE),
        sleepRemaining(0),
        ready(true) {
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
    bool isKernelThread;
    time_t timeSlice;
    time_t sleepRemaining;
    bool ready;
    static void contextSwitch(Context* oldRunning, Context* newRunning);
    static void wrapper();
};


#endif //_THREAD_HPP
