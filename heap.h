#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

typedef struct Block {
    int pid;
    size_t start;
    size_t size;
    int free;
    struct Block *next;
} Block;

extern Block *head;

void heap_init(void);
Block* heap_allocate(int pid, size_t size);
void heap_free(int pid);
void heap_compact(void);

#endif
