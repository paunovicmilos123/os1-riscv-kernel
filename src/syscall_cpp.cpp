#include "../h/syscall_cpp.hpp"

void* operator new(size_t size) {
    return mem_alloc(size);
}
void* operator new[](size_t size) {
    return mem_alloc(size);
}
void operator delete(void* ptr) noexcept {
    mem_free(ptr);
}
void operator delete[](void* ptr) noexcept {
    mem_free(ptr);
}


Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {

}

Thread::Thread() : myHandle(nullptr), body(nullptr), arg(this) {

}

Thread::~Thread() {

}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::start() {
    return thread_create(&this->myHandle, wrapper, this);
}

uint64 Thread::getThreadId() {
    return thread_id();
}

void Thread::wrapper(void *thread_handle) {
    Thread* t = (Thread*)thread_handle;
    if(t->body)
        t->body(t->arg);
    else
        t->run();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

int Thread::ping(Thread* t) {
    if(!t) return thread_ping(this->myHandle);
    return thread_ping(t->myHandle);
}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t t) {
    return sem_timedwait(myHandle, t);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}


PeriodicThread::PeriodicThread(time_t period) : Thread(PeriodicThread::wrapper, this) {
    if(period==0) period = 1;
    this->period = period;
}

void PeriodicThread::terminate() {
    this->period = 0;
}

void PeriodicThread::wrapper(void *thread_handle) {
    PeriodicThread* t = (PeriodicThread*)thread_handle;

    while(t->period) {
        t->periodicActivation();
        time_sleep(t->period);
    }
}


char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
