#include "../h/riscv.hpp"

#include "../h/syscall_c.hpp"
#include "../h/kConsole.hpp"

void userMain() {
    putc('z');
    putc('d');
    putc('d');
    putc('z');
    putc('\n');
}
void userMainWrapper(void *arg) {
    userMain();
}

void main() {
    kConsole::init();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    uint8* consoleHandlerStack = (uint8*)__mem_alloc(DEFAULT_STACK_SIZE);
    TCB::createThread(kConsole::handler, nullptr, consoleHandlerStack, true);

    TCB::running = TCB::createThread(nullptr, 0, 0);

    uint8 * userThreadStack = (uint8*) __mem_alloc(DEFAULT_STACK_SIZE);
    TCB* userThread = TCB::createThread(userMainWrapper, 0, userThreadStack);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while(!userThread->isFinished()) {
        thread_dispatch();
    }

    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    kSemaphore::deleteAll();
    Scheduler::killAll();
    delete TCB::running;
}