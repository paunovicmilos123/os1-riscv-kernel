#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

#include "../h/kernelPrinting.hpp"

TCB* TCB::running = nullptr;
time_t TCB::ticksRemaining = DEFAULT_TIME_SLICE;

TCB* TCB::createThread(Body body, void* arg, uint8* stack_space, bool isKernelThread) {
    return new TCB(body, arg, stack_space, isKernelThread);
}

void TCB::dispatch(bool dispatchToKernelThread) {
    TCB *old = running;
    if (!old->isFinished() && old->isReady()) { Scheduler::put(old, dispatchToKernelThread); }
    running = Scheduler::get(dispatchToKernelThread);

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::exit() {
    running->setFinished(true);
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
