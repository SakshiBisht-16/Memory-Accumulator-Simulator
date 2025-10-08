#ifndef HASH_H
#define HASH_H

#include "heap.h"

#define HASH_SIZE 100

void hash_init(void);
void hash_insert(int pid, Block *blk);
Block* hash_lookup(int pid);
void hash_delete(int pid);
int generate_pid(void);

#endif
