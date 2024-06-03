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
    static void* alloc(size_t);
    static int free(void*);

    static void init();
};


#endif
