#include "../h/kernelPrinting.hpp"
#include "../h/riscv.hpp"

#include "../h/syscall_c.hpp"

void s(void* arg) {
    kprintString("asd");
}

void main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    kprintString("started\n");

    thread_t handle;
    int i;
    int result = thread_create(&handle, s, &i);

    kprintString("created thread: ");
    kprintInt(result, 10, 1);
    kprintString("\n");

    thread_dispatch();

    kprintInt(thread_exit(), 10, 1);
    kprintString("\n");

    sem_t sem0 = (sem_t)15;
    kprintInt(sem_open(&sem0, 10), 10, 1);
    kprintString("\n");

    kprintInt(sem_close(sem0), 10, 1);
    kprintString("\n");

    kprintInt(sem_signal(sem0), 10, 1);
    kprintString("\n");

    kprintInt(sem_trywait(sem0), 10, 1);
    kprintString("\n");
}