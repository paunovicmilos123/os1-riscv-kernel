#include "../h/kernelPrinting.hpp"
#include "../h/riscv.hpp"

#include "../lib/console.h"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/printing.hpp"
#include "../h/kConsole.hpp"


int finished=0;

class FasterPeriodicWorker : public PeriodicThread {
public:
    FasterPeriodicWorker(time_t period) : PeriodicThread(period) {

    }
protected:
    void periodicActivation() override {
        static int i = 0;
        printString("faster: ");
        printInt(i++);
        printString("\n");
    }
};
class SlowerPeriodicWorker : public PeriodicThread {
public:
    SlowerPeriodicWorker(time_t period) : PeriodicThread(period) {

    }
protected:
    void periodicActivation() override {
        static int i = 0;
        printString("slower: ");
        printInt(i++);
        printString("\n");
    }
};

void workerA(void* arg) {
    for(int i=0; i<100; i++) {
        printInt((uint64)arg+10, 16);
        printString(" i: ");
        printInt(i);
        printString("\n");
        time_sleep(2);
    }
    finished++;
}

void userMainWrapper(void *arg) {
    thread_t threads[1];
    thread_create(threads, workerA, 0);
    PeriodicThread* p0 = new FasterPeriodicWorker(10);
    PeriodicThread* p1 = new SlowerPeriodicWorker(18);
    p0->start();
    p1->start();

    while(finished<1) {
        thread_dispatch();
    }

    p0->terminate();
    p1->terminate();
    delete p0;
    delete p1;
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