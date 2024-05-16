#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/kernelPrinting.hpp"
#include "../h/syscall_c.hpp"
#include "../lib/mem.h"

uint64 Riscv::ecall(uint64 code, uint64 arg0, uint64 arg1, uint64 arg2, uint64 arg3) {
    __asm__ volatile("ecall");
    uint64 ret;
    __asm__ volatile("mv %[val], a0": [val]"=r"(ret));
    return ret;
}

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrc sstatus, %[flag]" : : [flag] "r" (SSTATUS_SPP));
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap(uint64 syscall_code, uint64 arg0, uint64 arg1, uint64 arg2, uint64 arg3) {
    uint64 scause = r_scause();
    uint64 sepc = r_sepc();

    if(scause == ECALL_SUPERVISOR || scause == ECALL_USER) {
        uint64 volatile sepc = r_sepc()+4;
        switch (syscall_code) {
            case MEM_ALLOC:
                w_a0_context((uint64)__mem_alloc(arg0 * MEM_BLOCK_SIZE));
                break;
            case MEM_FREE:
                w_a0_context(__mem_free((void*) arg0));
                break;

            case THREAD_CREATE:
                *(thread_t*)arg0 = (thread_t)TCB::createThread((void(*)(void*))arg1, (void*)arg2, (uint8*)arg3);
                w_a0_context((*(thread_t*)arg0)?0:-2);
                break;
            default:
                kprintString("illegal syscall\n");
            case THREAD_EXIT:
                TCB::exit();
                w_a0_context((int)-1);
                break;
            case THREAD_DISPATCH:
                TCB::dispatch();
                break;

            case SEM_OPEN:
                kprintString("new semaphore: \n handle: ");
                kprintInt(arg0, 16);
                *((sem_t*)arg0) = (sem_t)0x554;
                kprintString("\n with initial value: ");
                kprintInt((uint)arg1, 10, 1);
                kprintString("\n");
                w_a0_context((int)-7);
                break;
            case SEM_CLOSE:
                kprintString("close semaphore: \n handle: ");
                kprintInt(arg0, 16);
                kprintString("\n");
                w_a0_context((int)-9);
                break;
            case SEM_WAIT:
                kprintString("wait on semaphore: ");
                kprintInt(arg0, 16);
                kprintString("\n");
                w_a0_context((int)-11);
                break;
            case SEM_SIGNAL:
                kprintString("signal on semaphore: ");
                kprintInt(arg0, 16);
                kprintString("\n");
                w_a0_context((int)-73);
                break;
            case SEM_TRYWAIT:
                kprintString("trywait on semaphore: ");
                kprintInt(arg0, 16);
                kprintString("\n");
                w_a0_context((int)-77);
                break;
            case GETC:
                kprintString("read char \n");
                w_a0_context((int)'a');
                break;
            case PUTC:
                kprintString("print char: ");
                __putc(arg0);
                kprintString("\n");
                break;
        }
        w_sepc(sepc);
    } else if(scause == INTERRUPT_TIMER) {
        mc_sip(SIP_SSIP);
    } else if (scause == INTERRUPT_CONSOLE) {
        console_handler();
    }else if (scause == ILLEGAL_INSTRUCTION) {
        kprintString("illegal instruction\n");
        Riscv::quit();
    } else {
        kprintString("unexpected trap cause\n");
        kprintString("scause: ");
        kprintInt(scause, 16);
        kprintString("\nsepc: ");
        kprintInt(sepc, 16);
        kprintString("\n");
        Riscv::quit();
    }
}