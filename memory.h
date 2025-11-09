#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void memory_init(void);
int memory_allocate_auto(size_t size);
int memory_free_pid(int pid);
void memory_compact(void);
const char* memory_get_map(void);

#endif
