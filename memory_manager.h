#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

#define TOTAL_MEMORY 1024
#define MAX_PROCESSES 100

typedef struct Block {
    int pid;
    size_t size;
    size_t start;
    size_t end;
    int allocated;
    struct Block *next;
} Block;

typedef struct {
    Block *head;
} Heap;

typedef struct Node {
    int pid;
    struct Node *next;
} Node;

typedef struct {
    Node *adjList[MAX_PROCESSES];
} Graph;

extern Heap heap;
extern Graph proc_graph;
extern int pid_counter;

/* Memory functions */
void heap_init(void);
int memory_allocate_auto(size_t size);
int memory_free_pid(int pid);
void memory_compact(void);
void display_memory(void);

/* Graph functions */
void graph_init(Graph *g);
void graph_add_edge(Graph *g, int from, int to);
void graph_display(Graph *g);

/* Persistence */
void save_heap(const char *filename);
void load_heap(const char *filename);
void save_graph(const char *filename);
void load_graph(const char *filename);

/* Session reset */
void reset_system(void);

#endif
