#ifndef KCONSOLE_HPP
#define KCONSOLE_HPP

#include "kSemaphore.hpp"

class kConsole {
public:
    static void putc(char);
    static char getc();
    static void handler(void* arg);

    static const uint BUFFER_SIZE = 100;
private:
    static char outputBuffer[];
    static uint64 outputBufferHead, outputBufferTail;
};


#endif
