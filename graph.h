#ifndef GRAPH_H
#define GRAPH_H

#define MAX_PROCESSES 300

typedef struct Node {
    int pid;
    struct Node *next;
} Node;

typedef struct {
    Node *adj[MAX_PROCESSES];
} Graph;

void graph_init(Graph *g);
void graph_add_edge(Graph *g, int from, int to);
void graph_display(Graph *g);
void graph_free_children(Graph *g, int parent_pid);

#endif
