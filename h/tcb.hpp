#ifndef _THREAD_HPP
#define _THREAD_HPP

#include "../lib/hw.h"
#include "../h/kAllocator.hpp"
#include "scheduler.hpp"
#include "kSemaphore.hpp"

class kSemaphore;

class TCB {
public:
    using Body = void (*)(void*);
    static TCB *createThread(Body body, void* arg, uint8* stack_space, bool isKernelThread=false);
    void* operator new(size_t size) {
        return kAllocator::alloc(size);
    }
    void operator delete(void* ptr) noexcept {
        kAllocator::free(ptr);
    }
    static void dispatch(bool dispatchToKernelThread=false);
    static void exit();

    inline bool isFinished() const {return finished;}
    inline void setFinished(bool finished) { TCB::finished = finished;}
    inline time_t getTimeSlice() const { return timeSlice; }
    inline bool isReady() const { return ready; }
    inline void setReady(bool ready) { this->ready = ready; }
    inline int ping() { if(finished) return -1; pinged = true; return 0; }
    void join(time_t t);

    static TCB* running;
    static time_t ticksRemaining;
private:
    friend class Riscv;
    friend class Scheduler;
    friend class kSemaphore;
    friend class SleepingList;
    friend class kAllocator;
    TCB(Body body, void* arg, uint8* stack_space, bool isKernelThread=false);

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
    uint64 id;
    uint64 blocks;
    bool pinged;
    kSemaphore* finishedSem;

    static uint64 numOfThreads;
    static void contextSwitch(Context* oldRunning, Context* newRunning);
    static void wrapper();
};


#endif //_THREAD_HPP
