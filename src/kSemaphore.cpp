#include "../h/kSemaphore.hpp"
#include "../h/riscv.hpp"

kSemaphore* kSemaphore::head = nullptr;

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

int kSemaphore::timedwait(time_t t) {
    if(!open) return SEM_DEAD;
    if(t==0) {
        return trywait()*-1;
    }

    int ret = 0;

    if(--value < 0) {
        timedBlock(t);
        TCB *old = TCB::running;
        TCB::running = Scheduler::get();
        Riscv::pushRegisters();
        TCB::contextSwitch(&old->context, &TCB::running->context);
        Riscv::popRegisters();
        if(TCB::running->sleepRemaining==0) ret = SEM_TIMEOUT;
        TCB::running->sleepRemaining = 0;
    }
    if(!open) return SEM_DEAD;
    return ret;
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
    if(++value <= 0) {
        if(!priority) {
            if (!unblock())
                sleepingList.wakeOne();
        } else {
            uint64 minBlocked = blocked ? blocked->id : ~0UL;
            uint64 minSleeping = sleepingList.getMinPriority();
            if(minSleeping>=minBlocked) unblock();
            else sleepingList.wakeOne(true);
        }
    }
    return 0;
}

void kSemaphore::block() {
    if(blocked==nullptr) {
        blocked = TCB::running;
        blocked->next = nullptr;
        return;
    }
    if(!priority) {
        TCB *last = blocked;
        while (last->next) {
            last = last->next;
        }
        last->next = TCB::running;
        TCB::running->next = nullptr;
    } else {
        TCB *t=blocked, *prev=nullptr;
        while(t) {
            if(TCB::running->id < t->id) break;
            prev = t;
            t = t->next;
        }
        TCB::running->next = t;
        if(prev) prev->next = TCB::running;
        else blocked = TCB::running;
    }
}

bool kSemaphore::unblock() {
    if(blocked) {
        TCB* ret = nullptr;
        ret = blocked;
        blocked = blocked->next;
        ret->next = nullptr;
        Scheduler::put(ret);
        return true;
    } else
        return false;
}

int kSemaphore::close() {
    while(blocked) {
        unblock();
    }
    sleepingList.wakeAll();
    if(!open)
        return SEM_DEAD;
    open = false;
    return 0;
}

void kSemaphore::timedBlock(time_t t) {
    sleepingList.addSleeping(t);
}

void kSemaphore::tick() {
    value += sleepingList.updateSleeping();
}

void kSemaphore::updateSleeping() {
    kSemaphore* t = kSemaphore::head;
    while(t) {
        t->tick();
        t = t->next;
    }
}

void kSemaphore::deleteAll() {
    kSemaphore* t;
    while(head) {
        t = head;
        head = head->next;
        delete t;
    }
}