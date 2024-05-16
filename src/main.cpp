#include "../h/kernelPrinting.hpp"
#include "../h/riscv.hpp"

#include "../h/syscall_c.hpp"

void workerA(void *arg) {
    kprintString("worker A: ");
    kprintInt(*(int*)arg);
    kprintString("\n");
    thread_exit();
}

void workerB(void *arg) {
    kprintString("worker B: ");
    char str[3] = {*(char*)arg, '\n'};
    kprintString(str);
    for(int i=0; i<3; i++) {
        kprintString("B: ");
        kprintInt(i);
        kprintString("\n");
        thread_dispatch();
    }
}

void main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    TCB::running = TCB::createThread(nullptr, 0, 0);

    thread_t threads[2];
    int arg_a = 1999;
    char arg_b = 'h';

    thread_create(&threads[0], workerA, &arg_a);
    thread_create(&threads[1], workerB, &arg_b);

    while(!(
            threads[0]->isFinished() &&
            threads[1]->isFinished())) {
        kprintString("sys\n");
        thread_dispatch();
    }
}