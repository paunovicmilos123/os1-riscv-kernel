#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP

#include "syscall_c.hpp"

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();

    int start ();

    static void dispatch ();
    static int sleep (time_t);

protected:
    Thread ();
    virtual void run () {}

private:
    thread_t myHandle;
    void (*body)(void*); void* arg;

    static void wrapper(void* thread_handle);
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int timedWait (time_t);
    int tryWait();
private:
    sem_t myHandle;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};

#endif
