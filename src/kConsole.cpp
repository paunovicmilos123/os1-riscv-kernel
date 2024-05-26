#include "../h/kConsole.hpp"
#include "../lib/hw.h"
#include "../h/syscall_c.hpp"

char kConsole::outputBuffer[kConsole::OUTPUT_BUFFER_SIZE];
uint64 kConsole::outputBufferHead = 0;
uint64 kConsole::outputBufferTail = 0;

char kConsole::inputBuffer[kConsole::INPUT_BUFFER_SIZE];
uint64 kConsole::inputBufferHead = 0;
uint64 kConsole::inputBufferTail = 0;

kSemaphore *kConsole::inputFull = nullptr;

void kConsole::init() {
    inputFull = new kSemaphore(0);
}

void kConsole::putc(char c) {
    outputBuffer[outputBufferHead++] = c;
    outputBufferHead %= OUTPUT_BUFFER_SIZE;
}

char kConsole::getc() {
    inputFull->wait();
    char ret = inputBuffer[inputBufferTail++];
    inputBufferTail %= INPUT_BUFFER_SIZE;
    return ret;
}

void kConsole::receiveChar(char c) {
    if(c=='\r') c = '\n';
    inputBuffer[inputBufferHead++] = c;
    inputBufferHead %= INPUT_BUFFER_SIZE;
    inputFull->signal();
}

bool kConsole::canReceive() {
    return (inputBufferHead+1)%INPUT_BUFFER_SIZE != inputBufferTail;
}

void kConsole::handler(void* arg) {
    while(true) {
        volatile uint8 *cstatus = (volatile uint8 *) CONSOLE_STATUS;
        volatile uint8 *ctx = (volatile uint8 *) CONSOLE_TX_DATA;

        while (*cstatus & CONSOLE_TX_STATUS_BIT && outputBufferHead != outputBufferTail) {
            *ctx = outputBuffer[outputBufferTail++];
            outputBufferTail %= OUTPUT_BUFFER_SIZE;
        }

        thread_dispatch();
    }
}