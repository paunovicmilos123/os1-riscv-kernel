#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP

#include "syscall_c.hpp"

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();

    int start ();
    uint64 getThreadId();

    static void dispatch ();
    static int sleep (time_t);
    int ping(Thread* t);
    void join(time_t t);

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
    Semaphore (unsigned init = 1, bool priority=false);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int timedWait (time_t);
    int tryWait();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    time_t period;
    static void wrapper(void* thread_handle);
};


class Console {
public:
    static char getc ();
    static void putc (char);
};

#endif
