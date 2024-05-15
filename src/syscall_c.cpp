#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

void* mem_alloc(size_t size) {
    return (void*) Riscv::ecall(MEM_ALLOC, size);
}

int mem_free(void* ptr) {
    return (int) Riscv::ecall(MEM_FREE, (uint64)ptr);
}

int thread_create(
        thread_t* handle,
        void(*start_routine)(void*),
        void* arg
) {
    void* stack_space = mem_alloc(100);
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

int sem_open(sem_t* handle, unsigned init) {
    return (int) Riscv::ecall(SEM_OPEN, (uint64)handle, (uint64)init);
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

int sem_trywait(sem_t id) {
    return (int) Riscv::ecall(SEM_TRYWAIT, (uint64)id);
}

char getc() {
    return (char) Riscv::ecall(GETC);
}

void putc(char c) {
    Riscv::ecall(PUTC, (uint64)c);
}