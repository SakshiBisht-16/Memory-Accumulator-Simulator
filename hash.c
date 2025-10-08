#include "hash.h"
#include <stdlib.h>

typedef struct HashNode {
    int pid;
    Block *blk;
    struct HashNode *next;
} HashNode;

HashNode *hash_table[HASH_SIZE];
static int next_pid = 100;

int hash_function(int pid) {
    return pid % HASH_SIZE;
}

void hash_init(void) {
    for (int i = 0; i < HASH_SIZE; i++)
        hash_table[i] = NULL;
}

void hash_insert(int pid, Block *blk) {
    int idx = hash_function(pid);
    HashNode *node = (HashNode*)malloc(sizeof(HashNode));
    node->pid = pid;
    node->blk = blk;
    node->next = hash_table[idx];
    hash_table[idx] = node;
}

Block* hash_lookup(int pid) {
    int idx = hash_function(pid);
    HashNode *curr = hash_table[idx];
    while (curr) {
        if (curr->pid == pid) return curr->blk;
        curr = curr->next;
    }
    return NULL;
}

void hash_delete(int pid) {
    int idx = hash_function(pid);
    HashNode *curr = hash_table[idx];
    HashNode *prev = NULL;
    while (curr) {
        if (curr->pid == pid) {
            if (prev) prev->next = curr->next;
            else hash_table[idx] = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int generate_pid(void) {
    return next_pid++;
}
