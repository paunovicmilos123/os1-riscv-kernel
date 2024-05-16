#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

#include "../h/kernelPrinting.hpp"

TCB* TCB::running = nullptr;

TCB* TCB::createThread(Body body, void* arg, uint8* stack_space) {
    return new TCB(body, arg, stack_space);
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::exit() {
    running->setFinished(true);
    TCB *old = running;
    running = Scheduler::get();
    __mem_free(old->stack);
    old->stack = nullptr;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::wrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    thread_exit();
}
