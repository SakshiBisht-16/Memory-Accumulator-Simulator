#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void memory_init(void);
int memory_allocate_auto(size_t size);   // returns PID
int memory_free_pid(int pid);             // returns 0 if success, -1 if fail
void memory_compact(void);

// Returns memory map as string
const char* memory_get_map(void);

#endif
