#include <stdio.h>
#include <stdlib.h>
#include "memory_manager.h"

Heap heap = { NULL };
Graph proc_graph;
int pid_counter = 1;

#define HEAP_FILE "/Users/sakshibisht/Documents/project/heap_state.dat"
#define GRAPH_FILE "/Users/sakshibisht/Documents/project/graph_state.dat"

/* Initialize heap with one large free block */
void heap_init(void) {
    heap.head = malloc(sizeof(Block));
    heap.head->pid = -1;
    heap.head->size = TOTAL_MEMORY;
    heap.head->allocated = 0;
    heap.head->start = 0;
    heap.head->end = TOTAL_MEMORY;
    heap.head->next = NULL;
}

/* Allocate memory using Best Fit (or First Fit simplified) */
int memory_allocate_auto(size_t size) {
    if (size == 0 || size > TOTAL_MEMORY)
        return -1;

    Block *curr = heap.head;

    while (curr) {
        if (!curr->allocated && curr->size >= size) {

            size_t original_end = curr->end; // remember current end before splitting

            // Split block if larger than needed
            if (curr->size > size) {
                Block *newBlock = malloc(sizeof(Block));
                newBlock->pid = -1;
                newBlock->allocated = 0;
                newBlock->size = curr->size - size;
                newBlock->start = curr->start + size;
                newBlock->end = original_end;
                newBlock->next = curr->next;

                curr->next = newBlock;
            }

            curr->allocated = 1;
            curr->pid = pid_counter++;
            curr->size = size;
            curr->end = curr->start + curr->size;

            return curr->pid;
        }

        curr = curr->next;
    }

    return -1; // No space found
}

/* Deallocate a block by PID */
int memory_free_pid(int pid) {
    Block *curr = heap.head;

    while (curr) {
        if (curr->allocated && curr->pid == pid) {
            curr->allocated = 0;
            curr->pid = -1;
            return 0;
        }
        curr = curr->next;
    }

    return -1; // Not found
}

/* Compact memory — move all allocated blocks to the start */
void memory_compact(void) {
    Block *curr = heap.head;
    size_t next_start = 0;
    Block *newHead = NULL;
    Block *last = NULL;

    // Move allocated blocks up
    while (curr) {
        if (curr->allocated) {
            curr->start = next_start;
            curr->end = curr->start + curr->size;
            next_start = curr->end;

            if (!newHead)
                newHead = curr;
            else
                last->next = curr;
            last = curr;
        }
        curr = curr->next;
    }

    // Add one large free block for remaining memory
    size_t free_space = TOTAL_MEMORY - next_start;
    Block *freeBlock = malloc(sizeof(Block));
    freeBlock->pid = -1;
    freeBlock->allocated = 0;
    freeBlock->size = free_space;
    freeBlock->start = next_start;
    freeBlock->end = next_start + free_space;
    freeBlock->next = NULL;

    if (last)
        last->next = freeBlock;
    else
        newHead = freeBlock;

    heap.head = newHead;
}

/* Display memory in tabular format */
void display_memory(void) {
    Block *curr = heap.head;

    printf("-------------------------------------------------------\n");
    printf("%-8s%-10s%-10s%-12s%-10s\n", "PID", "START", "END", "SIZE(KB)", "STATUS");
    printf("-------------------------------------------------------\n");

    while (curr) {
        printf("%-8d%-10zu%-10zu%-12zu%-10s\n",
               curr->pid,
               curr->start,
               curr->end,
               curr->size,
               curr->allocated ? "Used" : "Free");
        curr = curr->next;
    }

    printf("-------------------------------------------------------\n");
}

/* Save heap to file */
void save_heap(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    Block *curr = heap.head;
    while (curr) {
        fwrite(curr, sizeof(Block), 1, fp);
        curr = curr->next;
    }

    fclose(fp);
}

/* Load heap from file */
void load_heap(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        heap_init();
        return;
    }

    heap.head = NULL;
    Block *prev = NULL;
    Block temp;

    while (fread(&temp, sizeof(Block), 1, fp) == 1) {
        Block *newBlock = malloc(sizeof(Block));
        *newBlock = temp;
        newBlock->next = NULL;

        if (!heap.head)
            heap.head = newBlock;
        else
            prev->next = newBlock;
        prev = newBlock;
    }

    fclose(fp);

    pid_counter = 1;
    Block *c = heap.head;
    while (c) {
        if (c->allocated && c->pid >= pid_counter)
            pid_counter = c->pid + 1;
        c = c->next;
    }
}

/* Graph initialization */
void graph_init(Graph *g) {
    for (int i = 0; i < MAX_PROCESSES; i++)
        g->adjList[i] = NULL;
}

/* Add process relation */
void graph_add_edge(Graph *g, int from, int to) {
    if (from < 0 || from >= MAX_PROCESSES || to < 0 || to >= MAX_PROCESSES)
        return;

    Node *newNode = malloc(sizeof(Node));
    newNode->pid = to;
    newNode->next = g->adjList[from];
    g->adjList[from] = newNode;
}

/* Display graph */
void graph_display(Graph *g) {
    printf("\n--- Process Graph ---\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (g->adjList[i]) {
            printf("PID %d -> ", i);
            Node *curr = g->adjList[i];
            while (curr) {
                printf("%d ", curr->pid);
                curr = curr->next;
            }
            printf("\n");
        }
    }
}

/* Save and load graph */
void save_graph(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        Node *temp = proc_graph.adjList[i];
        while (temp) {
            fwrite(&i, sizeof(int), 1, fp);
            fwrite(&temp->pid, sizeof(int), 1, fp);
            temp = temp->next;
        }
    }

    fclose(fp);
}

void load_graph(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        graph_init(&proc_graph);
        return;
    }

    graph_init(&proc_graph);

    int from, to;
    while (fread(&from, sizeof(int), 1, fp) == 1) {
        if (fread(&to, sizeof(int), 1, fp) != 1)
            break;
        graph_add_edge(&proc_graph, from, to);
    }

    fclose(fp);
}

/* Reset system (on logout) */
void reset_system(void) {
    heap_init();
    graph_init(&proc_graph);
    pid_counter = 1;

    FILE *fp = fopen(HEAP_FILE, "wb");
    if (fp) fclose(fp);
    fp = fopen(GRAPH_FILE, "wb");
    if (fp) fclose(fp);
}



