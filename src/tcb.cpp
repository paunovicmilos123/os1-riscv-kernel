#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

#include "../h/kernelPrinting.hpp"

TCB* TCB::running = nullptr;
time_t TCB::ticksRemaining = DEFAULT_TIME_SLICE;
uint64 TCB::numOfThreads = 10;

TCB::TCB(Body body, void* arg, uint8* stack_space, bool isKernelThread) :
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
    ready(true),
    blocks(0),
    pinged(false) {
    if(body) {
        Scheduler::put(this);
        finishedSem = new kSemaphore(0);
    }
    if(!isKernelThread) id = numOfThreads++;
}



TCB* TCB::createThread(Body body, void* arg, uint8* stack_space, bool isKernelThread) {
    TCB* ret = new TCB(body, arg, stack_space, isKernelThread);
    return ret;
}

void TCB::dispatch(bool dispatchToKernelThread) {
    if(running->pinged) {
        running->pinged = false;
        kprintString("\n>>> thread ");
        kprintInt(running->id);
        kprintString(" pinged: ");
        kprintInt(running->blocks);
        kprintString(" <<<\n");
    }
    TCB *old = running;
    if (!old->isFinished() && old->isReady()) { Scheduler::put(old, dispatchToKernelThread); }
    running = Scheduler::get(dispatchToKernelThread);

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::exit() {
    running->setFinished(true);
    running->finishedSem->close();
    TCB *old = running;
    running = Scheduler::get();
    kAllocator::free(old->stack);
    old->stack = nullptr;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::wrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    thread_exit();
}

void TCB::join(time_t t) {
    if(t==0) finishedSem->wait();
    else finishedSem->timedwait(t);
}
