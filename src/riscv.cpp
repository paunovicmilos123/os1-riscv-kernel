#include "../h/riscv.hpp"
#include "../h/kConsole.hpp"
#include "../h/kernelPrinting.hpp"
#include "../h/syscall_c.hpp"
#include "../h/kAllocator.hpp"

uint64 Riscv::ecall(uint64 code, uint64 arg0, uint64 arg1, uint64 arg2, uint64 arg3) {
    __asm__ volatile("ecall");
    uint64 ret;
    __asm__ volatile("mv %[val], a0": [val]"=r"(ret));
    return ret;
}

void Riscv::popSppSpie()
{
    if(TCB::running->isKernelThread)
        __asm__ volatile ("csrs sstatus, %[flag]" : : [flag] "r" (SSTATUS_SPP));
    else
        __asm__ volatile ("csrc sstatus, %[flag]" : : [flag] "r" (SSTATUS_SPP));
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap(uint64 syscall_code, uint64 arg0, uint64 arg1, uint64 arg2, uint64 arg3) {
    uint64 scause = r_scause();
    uint64 sepc = r_sepc();

    if(scause == ECALL_SUPERVISOR || scause == ECALL_USER) {
        uint64 volatile sepc = r_sepc()+4;
        uint64 volatile sstatus = r_sstatus();
        switch (syscall_code) {
            case MEM_ALLOC:
                w_a0_context((uint64)kAllocator::alloc(arg0 * MEM_BLOCK_SIZE, true));
                break;
            case MEM_FREE:
                w_a0_context(kAllocator::free((void*) arg0, true));
                break;

            case THREAD_CREATE:
                *(TCB**)arg0 = (TCB*)TCB::createThread((void(*)(void*))arg1, (void*)arg2, (uint8*)arg3);
                w_a0_context((*(TCB**)arg0)?0:-2);
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
            case THREAD_ID:
                w_a0_context(TCB::running->id);
                break;
            case THREAD_PING:
                w_a0_context(((TCB*)arg0)->ping());
                break;
            case THREAD_JOIN:
                ((TCB*)arg0)->join(arg1);
                break;

            case SEM_OPEN:
                *((kSemaphore**)arg0) = (kSemaphore*)new kSemaphore((uint)arg1, (bool)arg2);
                w_a0_context((*(kSemaphore**)arg0)?0:-1);
                break;
            case SEM_CLOSE:
                w_a0_context((int)(
                        ((kSemaphore*)arg0)->close()
                ));
                break;
            case SEM_WAIT:
                w_a0_context((int)(
                        ((kSemaphore*)arg0)->wait()
                ));
                break;
            case SEM_SIGNAL:
                w_a0_context((int)(
                        ((kSemaphore*)arg0)->signal()
                ));
                break;
            case SEM_TIMEDWAIT:
                w_a0_context((int)(
                        ((kSemaphore*)arg0)->timedwait(arg1)
                        ));
                break;
            case SEM_TRYWAIT:
                w_a0_context((int)(
                        ((kSemaphore*)arg0)->trywait()
                ));
                break;

            case TIME_SLEEP:
                if(arg0==0) {
                    w_a0_context((int)-1);
                    break;
                }
                Scheduler::addSleeping(arg0);
                TCB::dispatch();
                w_a0_context((int)0);
                break;

            case GETC:
                w_a0_context((int)kConsole::getc());
                break;
            case PUTC:
                kConsole::putc(arg0);
                TCB::dispatch(true);
                break;
        }
        w_sepc(sepc);
        w_sstatus(sstatus);
    } else if(scause == INTERRUPT_TIMER) {
        mc_sip(SIP_SSIP);
        // nit kernela se izvrsava atomski
        if(!TCB::running->isKernelThread) {
            Scheduler::updateSleeping();
            kSemaphore::updateSleeping();
            if(TCB::ticksRemaining==0 || --TCB::ticksRemaining==0) {
                uint64 volatile sepc = r_sepc();
                uint64 volatile sstatus = r_sstatus();
                TCB::dispatch();
                w_sepc(sepc);
                w_sstatus(sstatus);
            }
        }
    } else if (scause == INTERRUPT_CONSOLE) {
        if (plic_claim() == CONSOLE_IRQ) {
            plic_complete(CONSOLE_IRQ);

            uint charsRead = 0;
            volatile uint8 *cstatus = (volatile uint8 *) CONSOLE_STATUS;
            volatile uint8 *crx = (volatile uint8 *) CONSOLE_RX_DATA;
            while(*cstatus & CONSOLE_RX_STATUS_BIT && kConsole::canReceive() && charsRead<kConsole::MAX_CHARS_TO_READ) {
                kConsole::receiveChar(*crx);
                charsRead++;
            }
        }
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