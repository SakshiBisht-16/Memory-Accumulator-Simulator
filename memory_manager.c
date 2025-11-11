#include <stdio.h>
#include <stdlib.h>
#include "memory_manager.h"

Heap heap = { NULL };
Graph proc_graph;
int pid_counter = 1;

/* Initialize heap */
void heap_init(void) {
    Block *curr = heap.head;
    while (curr) {
        Block *next = curr->next;
        free(curr);
        curr = next;
    }

    heap.head = malloc(sizeof(Block));
    heap.head->pid = -1;
    heap.head->size = TOTAL_MEMORY;
    heap.head->allocated = 0;
    heap.head->next = NULL;
}

/* Allocation logic */
int memory_allocate_auto(size_t size) {
    if (size <= 0 || size > TOTAL_MEMORY)
        return -1;

    Block *curr = heap.head;

    while (curr) {
        if (!curr->allocated && curr->size >= size) {

            if (curr->size > size) {
                Block *newBlock = malloc(sizeof(Block));
                newBlock->size = curr->size - size;
                newBlock->allocated = 0;
                newBlock->pid = -1;
                newBlock->next = curr->next;

                curr->next = newBlock;
                curr->size = size;
            }

            curr->allocated = 1;
            curr->pid = pid_counter++;
            return curr->pid;
        }
        curr = curr->next;
    }
    return -1;
}

/* Free memory by PID */
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
    return -1;
}

/* Compact memory */
void memory_compact(void) {
    Block *curr = heap.head;
    size_t free_space = 0;

    Block *newHead = NULL;
    Block *last = NULL;

    while (curr) {
        if (curr->allocated) {
            Block *newBlock = malloc(sizeof(Block));
            newBlock->pid = curr->pid;
            newBlock->size = curr->size;
            newBlock->allocated = 1;
            newBlock->next = NULL;

            if (!newHead)
                newHead = newBlock;
            else
                last->next = newBlock;

            last = newBlock;
        } else {
            free_space += curr->size;
        }
        curr = curr->next;
    }

    Block *freeBlock = malloc(sizeof(Block));
    freeBlock->pid = -1;
    freeBlock->size = free_space;
    freeBlock->allocated = 0;
    freeBlock->next = NULL;

    if (last)
        last->next = freeBlock;
    else
        newHead = freeBlock;

    curr = heap.head;
    while (curr) {
        Block *next = curr->next;
        free(curr);
        curr = next;
    }

    heap.head = newHead;
}

/* Display memory map */
void display_memory(void) {
    Block *curr = heap.head;
    printf("\n--- Memory Map ---\n");
    while (curr) {
        printf("[ %s | PID: %d | Size: %zuKB ] -> ",
               curr->allocated ? "Used" : "Free",
               curr->pid,
               curr->size);
        curr = curr->next;
    }
    printf("NULL\n");
}

/* Graph initialization */
void graph_init(Graph *g) {
    for (int i = 0; i < MAX_PROCESSES; i++)
        g->adjList[i] = NULL;
}

/* Add edge */
void graph_add_edge(Graph *g, int from, int to) {
    if (from < 0 || from >= MAX_PROCESSES ||
        to < 0 || to >= MAX_PROCESSES)
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

/* Save heap to file */
void save_heap(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    Block *curr = heap.head;
    while (curr) {
        fwrite(&curr->pid, sizeof(int), 1, fp);
        fwrite(&curr->size, sizeof(size_t), 1, fp);
        fwrite(&curr->allocated, sizeof(int), 1, fp);
        curr = curr->next;
    }

    fclose(fp);
}

/* Load heap WITHOUT resetting on empty file */
void load_heap(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        heap_init();
        return;
    }

    Block *prev = NULL;
    heap.head = NULL;

    while (1) {
        Block *newBlock = malloc(sizeof(Block));
        if (fread(&newBlock->pid, sizeof(int), 1, fp) != 1) {
            free(newBlock);
            break;
        }
        fread(&newBlock->size, sizeof(size_t), 1, fp);
        fread(&newBlock->allocated, sizeof(int), 1, fp);
        newBlock->next = NULL;

        if (prev)
            prev->next = newBlock;
        else
            heap.head = newBlock;

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

/* Save graph */
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

/* Load graph safely */
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

/* Reset system on logout */
void reset_system(void) {
    heap_init();
    graph_init(&proc_graph);
    pid_counter = 1;

    FILE *fp = fopen("heap_state.dat", "wb");
    if (fp) fclose(fp);

    fp = fopen("graph_state.dat", "wb");
    if (fp) fclose(fp);
}
