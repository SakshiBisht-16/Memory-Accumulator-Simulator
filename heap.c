#include "heap.h"
#include <stdlib.h>
#include <string.h>

#define TOTAL_MEMORY 256

Block *head = NULL;

void heap_init(void) {
    head = (Block*)malloc(sizeof(Block));
    head->pid = -1;
    head->start = 0;
    head->size = TOTAL_MEMORY;
    head->free = 1;
    head->next = NULL;
}

Block* heap_allocate(int pid, size_t size) {
    Block *curr = head;
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size) {
                Block *new_blk = (Block*)malloc(sizeof(Block));
                new_blk->pid = -1;
                new_blk->start = curr->start + size;
                new_blk->size = curr->size - size;
                new_blk->free = 1;
                new_blk->next = curr->next;
                curr->next = new_blk;
                curr->size = size;
            }
            curr->pid = pid;
            curr->free = 0;
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void heap_free(int pid) {
    Block *curr = head;
    while (curr) {
        if (curr->pid == pid) {
            curr->pid = -1;
            curr->free = 1;
            break;
        }
        curr = curr->next;
    }
    // merge adjacent free blocks
    curr = head;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            Block *tmp = curr->next;
            curr->size += tmp->size;
            curr->next = tmp->next;
            free(tmp);
        } else {
            curr = curr->next;
        }
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
    // move all free memory to the end
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
        Block *free_blk = (Block*)malloc(sizeof(Block));
        free_blk->pid = -1;
        free_blk->start = current_start;
        free_blk->size = TOTAL_MEMORY - current_start;
        free_blk->free = 1;
        free_blk->next = NULL;
        if (!new_head) new_head = free_blk;
        else { tail->next = free_blk; tail = free_blk; }
    } else {
        tail->next = NULL;
    }
    head = new_head;
}
