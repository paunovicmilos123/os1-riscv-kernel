#include "../h/kernelPrinting.hpp"
#include "../h/riscv.hpp"

#include "../lib/console.h"
#include "../h/syscall_c.hpp"
#include "../h/printing.hpp"
#include "../h/kConsole.hpp"

sem_t mutex;
bool finishedA = false, finishedB = false, finishedC = false;
void workerA(void *arg) {
    int i = sem_trywait(mutex);
    printString("A wait: ");
    printInt(i, 10, 1);
    printString("\n");
    for(int i=0; i<10; i++)
        thread_dispatch();
    i = sem_signal(mutex);
    printString("A signal: ");
    printInt(i, 10, 1);
    printString("\n");
    finishedA = true;
}
void workerB(void *arg) {
    int i = sem_wait(mutex);
    printString("B wait: ");
    printInt(i, 10, 1);
    printString("\n");
    thread_dispatch();
    i = sem_signal(mutex);
    printString("B signal: ");
    printInt(i, 10, 1);
    printString("\n");
    finishedB = true;
}
void workerC(void *arg) {
    int i = sem_trywait(mutex);
    printString("C trywait: ");
    printInt(i, 10, 1);
    printString("\n");
    thread_dispatch();
    i = sem_signal(mutex);
    printString("C signal: ");
    printInt(i, 10, 1);
    printString("\n");
    for(int k=0; k<100000; k++);
    i = sem_close(mutex);
    printString("C close: ");
    printInt(i, 10, 1);
    printString("\n");
    finishedB = true;
}

void userMain(void *arg) {
    printString("semaphore initial value? ");
    char c = '1';//getc();
    printString("sem_open with ");
    putc(c);
    printString("\n");


    sem_open(&mutex, c - '0');

    thread_t threads[3];
    thread_create(&threads[0], workerA, nullptr);
    thread_create(&threads[1], workerB, nullptr);
    thread_create(&threads[2], workerC, nullptr);

    while(!(threads[0]->isFinished() && threads[1]->isFinished() && threads[2]->isFinished())) {
        //kprintString("user main\n");
        thread_dispatch();
    }
}


void main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    uint8* console_thread_stack = (uint8*)__mem_alloc(DEFAULT_STACK_SIZE);
    TCB::createThread(kConsole::handler, nullptr, console_thread_stack, true);
    TCB::running = TCB::createThread(nullptr, 0, 0);

    thread_t user_thread;
    thread_create(&user_thread, userMain, nullptr);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while(!user_thread->isFinished()) {
        thread_dispatch();
    }
}