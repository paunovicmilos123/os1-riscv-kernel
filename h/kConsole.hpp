#ifndef KCONSOLE_HPP
#define KCONSOLE_HPP

#include "kSemaphore.hpp"

class kConsole {
public:
    static void init();
    static void putc(char);
    static char getc();
    static void receiveChar(char);
    static bool canReceive();
    static void handler(void* arg);

    static const uint OUTPUT_BUFFER_SIZE = 100, INPUT_BUFFER_SIZE = 100;
    static const uint MAX_CHARS_TO_READ = 30;
private:
    static char outputBuffer[];
    static uint64 outputBufferHead, outputBufferTail;
    static char inputBuffer[];
    static uint64 inputBufferHead, inputBufferTail;
    static kSemaphore* inputFull;
};


#endif
