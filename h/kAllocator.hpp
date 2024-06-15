#ifndef _KALLOCATOR_HPP
#define _KALLOCATOR_HPP

#include "../lib/hw.h"

struct FreeMem {
    FreeMem* prev;
    FreeMem* next;
    size_t size;
};
struct BlockHeader {
    size_t size;
};

class kAllocator {
private:
    static FreeMem* freeMemHead;
    static void tryToJoin(FreeMem*);
public:
    static void* alloc(size_t s, bool syscall=false);
    static int free(void* ptr, bool syscall=false);

    static void init();
};


#endif
