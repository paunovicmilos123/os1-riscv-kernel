#include "../h/kernelPrinting.hpp"
#include "../h/riscv.hpp"

#include "../lib/console.h"
#include "../h/syscall_c.hpp"

sem_t mutex;
bool finishedA = false, finishedB = false, finishedC = false;
void workerA(void *arg) {
    int i = sem_trywait(mutex);
    kprintString("A wait: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    for(int i=0; i<10; i++)
        thread_dispatch();
    i = sem_signal(mutex);
    kprintString("A signal: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    finishedA = true;
}
void workerB(void *arg) {
    int i = sem_wait(mutex);
    kprintString("B wait: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    thread_dispatch();
    i = sem_signal(mutex);
    kprintString("B signal: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    finishedB = true;
}
void workerC(void *arg) {
    int i = sem_trywait(mutex);
    kprintString("C trywait: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    thread_dispatch();
    i = sem_signal(mutex);
    kprintString("C signal: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    i = sem_close(mutex);
    kprintString("C close: ");
    kprintInt(i, 10, 1);
    kprintString("\n");
    finishedB = true;
}

void userMain(void *arg) {

    sem_open(&mutex, 1);

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
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    TCB::running = TCB::createThread(nullptr, 0, 0);

    thread_t user_thread;
    thread_create(&user_thread, userMain, nullptr);

    while(!user_thread->isFinished()) {
        thread_dispatch();
    }
}