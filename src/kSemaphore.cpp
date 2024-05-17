#include "../h/kSemaphore.hpp"
#include "../h/riscv.hpp"

int kSemaphore::wait() {
    if(!open) return SEM_DEAD;
    if(--value < 0) {
        block();
        TCB *old = TCB::running;
        TCB::running = Scheduler::get();
        Riscv::pushRegisters();
        TCB::contextSwitch(&old->context, &TCB::running->context);
        Riscv::popRegisters();
    }
    if(!open) return SEM_DEAD;
    return 0;
}

int kSemaphore::trywait() {
    if(!open) return SEM_DEAD;
    if(value<=0)
        return 1;
    value--;
    return 0;
}

int kSemaphore::signal() {
    if(!open) return SEM_DEAD;
    if(++value <= 0)
        unblock();
    return 0;
}

void kSemaphore::block() {
    if(blocked==nullptr) {
        blocked = TCB::running;
        blocked->next = nullptr;
        return;
    }
    TCB* last = blocked;
    while(last->next) {
        last = last->next;
    }
    last->next = TCB::running;
    TCB::running->next = nullptr;
}

void kSemaphore::unblock() {
    TCB* ret = nullptr;
    if(blocked) {
        ret = blocked;
        blocked = blocked->next;
        ret->next = nullptr;
        Scheduler::put(ret);
    }
}

int kSemaphore::close() {
    while(blocked) {
        unblock();
    }
    if(!open)
        return SEM_DEAD;
    open = false;
    return 0;
}
