#include "graph.h"
#include "hash.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

void graph_init(Graph *g) {
    for (int i = 0; i < MAX_PROCESSES; i++)
        g->adj[i] = NULL;
}

void graph_add_edge(Graph *g, int from, int to) {
    Node *n = malloc(sizeof(Node));
    n->pid = to;
    n->next = g->adj[from];
    g->adj[from] = n;
}

void graph_display(Graph *g) {
    printf("\n=== PROCESS RELATIONSHIP GRAPH ===\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (g->adj[i]) {
            printf("Process %d -> ", i);
            Node *curr = g->adj[i];
            while (curr) {
                printf("%d ", curr->pid);
                curr = curr->next;
            }
            printf("\n");
        }
    }
}

void graph_free_children(Graph *g, int parent_pid) {
    Node *curr = g->adj[parent_pid];
    while (curr) {
        int child_pid = curr->pid;
        printf("   -> Deallocating child process %d\n", child_pid);

        graph_free_children(g, child_pid);
        memory_free_pid(child_pid);

        curr = curr->next;
    }

    Node *tmp = g->adj[parent_pid];
    while (tmp) {
        Node *to_free = tmp;
        tmp = tmp->next;
        free(to_free);
    }
    g->adj[parent_pid] = NULL;
}
