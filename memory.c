#include "memory.h"
#include "heap.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char map_buffer[1024];

void memory_init(void) {
    heap_init();
    hash_init();
}

int memory_allocate_auto(size_t size) {
    int pid = generate_pid();
    Block *blk = heap_allocate(pid, size);
    if (blk) {
        hash_insert(pid, blk);
        return pid;
    }
    return -1;
}

int memory_free_pid(int pid) {
    Block *blk = hash_lookup(pid);
    if (blk) {
        heap_free(pid);
        hash_delete(pid);
        return 0;
    }
    return -1;
}

void memory_compact(void) {
    heap_compact();
}

const char* memory_get_map(void) {
    Block *curr = head;
    strcpy(map_buffer, "PID\tStart\tSize\tFree\n");
    while (curr) {
        char line[64];
        sprintf(line, "%d\t%zu\t%zu\t%s\n",
                curr->pid, curr->start, curr->size,
                curr->free ? "TRUE" : "FALSE");
        strcat(map_buffer, line);
        curr = curr->next;
    }
    return map_buffer;
}
