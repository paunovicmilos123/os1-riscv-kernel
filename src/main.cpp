#include "../h/kernelPrinting.hpp"
#include "../h/riscv.hpp"

#include "../lib/console.h"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/printing.hpp"
#include "../h/kConsole.hpp"

sem_t sem;
int finished;
void worker(void* arg) {
    int id = (uint64)arg;
    printInt(10+(uint64)arg, 16);
    printString("\n");
    int ret = sem_timedwait(sem, 6*id);
    printInt(10+(uint64)arg, 16);
    printString(" sem: ");
    printInt(ret, 10, 1);
    printString("\n");

    for(int i=0; i<10; i++) {
        for(int j=0; j<10000; j++)
            for(int k=0; k<5000; k++);
        printInt(10+(uint64)arg, 16);
        printString(": ");
        printInt(i);
        printString("\n");
    }

    if(ret==0) sem_signal(sem);
    finished++;
}
void sleepy(void* arg) {
    printString("sleepy\n");
    time_sleep(4);
    printString("woken\n");
}

void userMainWrapper(void *arg) {
    sem_open(&sem, 1);
    finished = 0;
    thread_t sleepyThreads[1];
    for(uint64 i=0; i<sizeof(sleepyThreads)/sizeof(thread_t); i++)
        thread_create(sleepyThreads+i, sleepy, (void*)i);

    thread_t threads[3];
    for(uint64 i=0; i<sizeof(threads)/sizeof(thread_t); i++)
        thread_create(threads+i, worker, (void*)i);


    while(finished<3) {
        thread_dispatch();
    }
}


void main() {
    kConsole::init();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    uint8* console_thread_stack = (uint8*)__mem_alloc(DEFAULT_STACK_SIZE);
    TCB::createThread(kConsole::handler, nullptr, console_thread_stack, true);
    TCB::running = TCB::createThread(nullptr, 0, 0);

    thread_t user_thread;
    thread_create(&user_thread, userMainWrapper, nullptr);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while(!user_thread->isFinished()) {
        thread_dispatch();
    }
}