#include "heap.h"
#include "bst.h"
#include <stdlib.h>
#include <string.h>

#define TOTAL_MEMORY 1024

Block *head = NULL;
BSTNode *free_tree = NULL;

void heap_init(void) {
    head = malloc(sizeof(Block));
    head->pid = -1;
    head->start = 0;
    head->size = TOTAL_MEMORY;
    head->free = 1;
    head->next = NULL;
    free_tree = bst_insert(NULL, head);
}

Block* heap_allocate(int pid, size_t size) {
    Block *best = bst_find_best_fit(free_tree, size);
    if (!best) return NULL;

    free_tree = bst_delete(free_tree, best->size);

    if (best->size > size) {
        Block *new_blk = malloc(sizeof(Block));
        new_blk->pid = -1;
        new_blk->start = best->start + size;
        new_blk->size = best->size - size;
        new_blk->free = 1;
        new_blk->next = best->next;
        best->next = new_blk;
        best->size = size;
        free_tree = bst_insert(free_tree, new_blk);
    }

    best->pid = pid;
    best->free = 0;
    return best;
}

void heap_free(int pid) {
    Block *curr = head;
    while (curr) {
        if (curr->pid == pid) {
            curr->pid = -1;
            curr->free = 1;
            free_tree = bst_insert(free_tree, curr);
            break;
        }
        curr = curr->next;
    }
}

void heap_compact(void) {
    size_t current_start = 0;
    Block *curr = head;
    while (curr) {
        if (!curr->free) {
            curr->start = current_start;
            current_start += curr->size;
        }
        curr = curr->next;
    }

    Block *new_head = NULL, *tail = NULL;
    curr = head;
    while (curr) {
        Block *next = curr->next;
        if (!curr->free) {
            if (!new_head) new_head = tail = curr;
            else { tail->next = curr; tail = curr; }
        }
        curr = next;
    }

    if (current_start < TOTAL_MEMORY) {
        Block *free_blk = malloc(sizeof(Block));
        free_blk->pid = -1;
        free_blk->start = current_start;
        free_blk->size = TOTAL_MEMORY - current_start;
        free_blk->free = 1;
        free_blk->next = NULL;
        if (!new_head) new_head = free_blk;
        else { tail->next = free_blk; tail = free_blk; }
        bst_clear(free_tree);
        free_tree = bst_insert(NULL, free_blk);
    } else {
        tail->next = NULL;
    }
    head = new_head;
}
