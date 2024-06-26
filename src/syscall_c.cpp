#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

void* mem_alloc(size_t size) {
    if(size==0) return nullptr;
    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return (void*) Riscv::ecall(MEM_ALLOC, blocks);
}

int mem_free(void* ptr) {
    return (int) Riscv::ecall(MEM_FREE, (uint64)ptr);
}

int thread_create(
        thread_t* handle,
        void(*start_routine)(void*),
        void* arg
) {
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if(stack_space == nullptr) return -1;
    return (int) Riscv::ecall(THREAD_CREATE,
                              (uint64)handle,
                              (uint64)start_routine,
                              (uint64)arg,
                              (uint64)stack_space);
}

int thread_exit() {
    return (int) Riscv::ecall(THREAD_EXIT);
}

void thread_dispatch() {
    Riscv::ecall(THREAD_DISPATCH);
}

uint64 thread_id() {
    return (uint64) Riscv::ecall(THREAD_ID);
}

int thread_ping(thread_t handle) {
    return (int) Riscv::ecall(THREAD_PING, (uint64)handle);
}

void thread_join(thread_t handle, time_t t) {
    Riscv::ecall(THREAD_JOIN, (uint64)handle, t);
}

int sem_open(sem_t* handle, unsigned init, bool priority) {
    return (int) Riscv::ecall(SEM_OPEN, (uint64)handle, (uint64)init, (uint64)priority);
}

int sem_close (sem_t handle) {
    return (int) Riscv::ecall(SEM_CLOSE, (uint64)handle);
}

int sem_wait (sem_t id) {
    return (int) Riscv::ecall(SEM_WAIT, (uint64)id);
}

int sem_signal (sem_t id) {
    return (int) Riscv::ecall(SEM_SIGNAL, (uint64)id);
}

int sem_timedwait(sem_t id, time_t timeout) {
    return (int) Riscv::ecall(SEM_TIMEDWAIT, (uint64)id, (uint64)timeout);
}

int sem_trywait(sem_t id) {
    return (int) Riscv::ecall(SEM_TRYWAIT, (uint64)id);
}

int time_sleep(time_t t) {
    return (int) Riscv::ecall(TIME_SLEEP, (uint64)t);
}

char getc() {
    return (char) Riscv::ecall(GETC);
}

void putc(char c) {
    Riscv::ecall(PUTC, (uint64)c);
}