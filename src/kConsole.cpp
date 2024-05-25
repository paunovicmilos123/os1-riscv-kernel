#include "../h/kConsole.hpp"
#include "../lib/hw.h"
#include "../h/syscall_c.hpp"

char kConsole::outputBuffer[100];
uint64 kConsole::outputBufferHead = 0;
uint64 kConsole::outputBufferTail = 0;

void kConsole::putc(char c) {
    outputBuffer[outputBufferHead++] = c;
    outputBufferHead %= BUFFER_SIZE;
}

char kConsole::getc() {
    return 0;
}

void kConsole::handler(void* arg) {
    while(true) {
        volatile uint8 *cstatus = (volatile uint8 *) CONSOLE_STATUS;
        volatile uint8 *ctx = (volatile uint8 *) CONSOLE_TX_DATA;

        while (*cstatus & CONSOLE_TX_STATUS_BIT && outputBufferHead != outputBufferTail) {
            *ctx = outputBuffer[outputBufferTail++];
            outputBufferTail %= BUFFER_SIZE;
        }

        thread_dispatch();
    }
}