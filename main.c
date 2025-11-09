#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "hash.h"
#include "memory.h"
#include "graph.h"

Graph proc_graph;

void display_memory(void) {
    printf("\n================== MEMORY TABLE ==================\n");
    printf("| %-8s | %-15s | %-10s | %-6s |\n", "PID", "START", "SIZE(KB)", "STATUS");
    printf("--------------------------------------------------\n");

    Block *curr = head;
    while (curr) {
        printf("| %-8d | %-15zu | %-10zu | %-6s |\n",
               curr->pid == -1 ? 0 : curr->pid,
               curr->start, curr->size,
               curr->free ? "Free" : "Allocated");
        curr = curr->next;
    }
    printf("==================================================\n");
}

int main(void) {
    int choice, pid;
    size_t size;

    heap_init();
    hash_init();
    graph_init(&proc_graph);

    printf("========================================\n");
    printf("   MEMORY ACCUMULATOR SIMULATOR v2.0\n");
    printf("========================================\n");

    while (1) {
        printf("\n-------------- MENU --------------\n");
        printf("1. Allocate Memory\n");
        printf("2. Deallocate Memory (Cascade)\n");
        printf("3. Compact Memory\n");
        printf("4. Display Memory\n");
        printf("5. Add Process Relationship\n");
        printf("6. Display Process Graph\n");
        printf("7. Exit\n");
        printf("---------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nEnter size to allocate (KB): ");
                scanf("%zu", &size);
                pid = memory_allocate_auto(size);
                if (pid != -1)
                    printf("Allocated %zuKB to Process PID: %d\n", size, pid);
                else
                    printf("Allocation failed.\n");
                break;

            case 2:
                printf("\nEnter Process ID to deallocate: ");
                scanf("%d", &pid);
                if (memory_free_pid(pid) == -1)
                    printf("Process %d not found.\n", pid);
                display_memory();
                break;

            case 3:
                printf("\nCompacting memory...\n");
                memory_compact();
                printf("Memory compacted successfully.\n");
                display_memory();
                break;

            case 4:
                display_memory();
                break;

            
            case 5: {
                int from, to;
                printf("Enter parent PID: ");
                scanf("%d", &from);
                printf("Enter child PID: ");
                scanf("%d", &to);
                graph_add_edge(&proc_graph, from, to);
                printf("Relationship added.\n");
                break;
            }

            case 6:
                graph_display(&proc_graph);
                break;

            case 7:
                printf("\nExiting simulator... Goodbye!\n");
                exit(0);

            default:
                printf("Invalid choice.\n");
        }
    }
}
