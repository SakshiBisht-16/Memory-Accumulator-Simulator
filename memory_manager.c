#include <stdio.h>
#include <stdlib.h>
#include "memory_manager.h"

Heap heap;
Graph proc_graph;

void heap_init(void) {
    heap.head = (Block*)malloc(sizeof(Block));
    heap.head->pid = -1;
    heap.head->size = TOTAL_MEMORY;
    heap.head->allocated = 0;
    heap.head->next = NULL;
}

int memory_allocate_auto(size_t size) {
    static int pid_counter = 1;
    Block* curr = heap.head;

    while (curr) {
        if (!curr->allocated && curr->size >= size) {
            if (curr->size > size) {
                Block* newBlock = (Block*)malloc(sizeof(Block));
                newBlock->size = curr->size - size;
                newBlock->pid = -1;
                newBlock->allocated = 0;
                newBlock->next = curr->next;

                curr->next = newBlock;
                curr->size = size;
            }
            curr->pid = pid_counter++;
            curr->allocated = 1;
            return curr->pid;
        }
        curr = curr->next;
    }
    return -1;
}

int memory_free_pid(int pid) {
    Block* curr = heap.head;
    while (curr) {
        if (curr->allocated && curr->pid == pid) {
            curr->allocated = 0;
            curr->pid = -1;
            return 0;
        }
        curr = curr->next;
    }
    return -1;
}

void memory_compact(void) {
    Block* curr = heap.head;
    size_t free_space = 0;

    Block* newHead = NULL;
    Block* last = NULL;

    while (curr) {
        if (curr->allocated) {
            Block* newBlock = (Block*)malloc(sizeof(Block));
            *newBlock = *curr;
            newBlock->next = NULL;

            if (!newHead) newHead = newBlock;
            else last->next = newBlock;
            last = newBlock;
        } else {
            free_space += curr->size;
        }
        curr = curr->next;
    }

    Block* freeBlock = (Block*)malloc(sizeof(Block));
    freeBlock->pid = -1;
    freeBlock->size = free_space;
    freeBlock->allocated = 0;
    freeBlock->next = NULL;

    if (last) last->next = freeBlock;
    else newHead = freeBlock;

    heap.head = newHead;
}

void display_memory(void) {
    Block* curr = heap.head;
    printf("\n--- Memory Map ---\n");
    while (curr) {
        printf("[ %s | %d | %zuKB ] -> ",
               curr->allocated ? "Used" : "Free", curr->pid, curr->size);
        curr = curr->next;
    }
    printf("NULL\n");
}

// ============ GRAPH MANAGEMENT ============

void graph_init(Graph* g) {
    for (int i = 0; i < 100; i++)
        g->adjList[i] = NULL;
}

void graph_add_edge(Graph* g, int from, int to) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->pid = to;
    newNode->next = g->adjList[from];
    g->adjList[from] = newNode;
}

void graph_display(Graph* g) {
    printf("\n--- Process Graph ---\n");
    for (int i = 0; i < 100; i++) {
        if (g->adjList[i]) {
            printf("PID %d -> ", i);
            Node* temp = g->adjList[i];
            while (temp) {
                printf("%d ", temp->pid);
                temp = temp->next;
            }
            printf("\n");
        }
    }
}
