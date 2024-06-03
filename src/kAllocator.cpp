#include "../h/kAllocator.hpp"

FreeMem* kAllocator::freeMemHead = nullptr;

void* kAllocator::alloc(size_t s) {
    size_t minBytes = s + sizeof(BlockHeader);
    size_t neededBlocks = (minBytes+MEM_BLOCK_SIZE-1) / MEM_BLOCK_SIZE;
    size_t neededBytes = neededBlocks * MEM_BLOCK_SIZE;

    size_t ret = 0;
    FreeMem* t = freeMemHead;
    while(t) {
        if(t->size >= neededBytes) {
            size_t rest = t->size - neededBytes;

            if(rest>0) {
                FreeMem *newFreeSeg = (FreeMem *) ((uint64) t + neededBytes);
                newFreeSeg->prev = t->prev;
                newFreeSeg->next = t->next;
                newFreeSeg->size = rest;
                if(!newFreeSeg->prev) freeMemHead = newFreeSeg;
            } else {
                if(t->prev) t->prev->next = t->next;
                else freeMemHead = t->next;

                if(t->next) t->next->prev = t->prev;
            }
            BlockHeader* header = (BlockHeader*)t;
            header->size = neededBytes;
            ret = (size_t)header + sizeof(BlockHeader);

            break;
        }
        t = t->next;
    }

    return (void*)ret;
}

int kAllocator::free(void *ptr) {
    BlockHeader* header = (BlockHeader*)((size_t)ptr - sizeof(BlockHeader));
    size_t size = header->size;
    size_t addr = (size_t)header;

    FreeMem* cur = nullptr;
    if(freeMemHead && addr>(size_t)freeMemHead)
        for(cur=freeMemHead; cur->next && addr>(size_t)(cur->next); cur=cur->next);

    FreeMem* newSeg = (FreeMem*)addr;
    newSeg->size = size;
    newSeg->prev = cur;

    if(cur) newSeg->next = cur->next;
    else newSeg->next = freeMemHead;

    if(newSeg->next) newSeg->next->prev = newSeg;
    if(cur) cur->next = newSeg;
    else freeMemHead = newSeg;

    tryToJoin(newSeg);
    tryToJoin(cur);

    return 0;
}

void kAllocator::tryToJoin(FreeMem* t) {
    if(!t || !t->next) return;
    if((size_t)t+t->size != (size_t)t->next) return;

    t->size += t->next->size;
    t->next = t->next->next;
    if(t->next) t->next->prev = t;
}

void kAllocator::init() {
    size_t heap_start = (size_t) HEAP_START_ADDR;
    heap_start += MEM_BLOCK_SIZE-1;
    heap_start = heap_start / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE;

    size_t heap_size = (size_t)HEAP_END_ADDR - heap_start;
    heap_size = heap_size / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE;

    freeMemHead = (FreeMem*)heap_start;
    freeMemHead->size = heap_size;
    freeMemHead->prev = freeMemHead->next = nullptr;
}
