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

void Thread::wrapper(void *thread_handle) {
    Thread* t = (Thread*)thread_handle;
    if(t->body)
        t->body(t->arg);
    else
        t->run();
}

int Thread::sleep(time_t t) {
    return 0;
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
    return 0;
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}





char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
