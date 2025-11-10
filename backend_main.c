#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

// Global structures
extern Heap heap;
extern Graph proc_graph;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./backend_main <command> [args]\n");
        return 1;
    }

    // Initialize heap and process graph
    heap_init();
    graph_init(&proc_graph);

    // COMMAND: allocate <size>
    if (strcmp(argv[1], "allocate") == 0 && argc == 3) {
        size_t size = atoi(argv[2]);
        int pid = memory_allocate_auto(size);
        if (pid != -1)
            printf("✅ Allocated %zuKB to Process PID: %d\n", size, pid);
        else
            printf("❌ Allocation failed.\n");
        display_memory();
    }

    // COMMAND: deallocate <pid>
    else if (strcmp(argv[1], "deallocate") == 0 && argc == 3) {
        int pid = atoi(argv[2]);
        if (memory_free_pid(pid) == -1)
            printf("⚠️ Process %d not found.\n", pid);
        else
            printf("🗑️ Deallocated PID: %d\n", pid);
        display_memory();
    }

    // COMMAND: compact
    else if (strcmp(argv[1], "compact") == 0) {
        printf("🧹 Compacting memory...\n");
        memory_compact();
        printf("✅ Memory compacted successfully.\n");
        display_memory();
    }

    // COMMAND: display
    else if (strcmp(argv[1], "display") == 0) {
        display_memory();
    }

    // COMMAND: add_relation <from> <to>
    else if (strcmp(argv[1], "add_relation") == 0 && argc == 4) {
        int from = atoi(argv[2]);
        int to = atoi(argv[3]);
        graph_add_edge(&proc_graph, from, to);
        printf("🔗 Relationship added: %d → %d\n", from, to);
    }

    // COMMAND: show_graph
    else if (strcmp(argv[1], "show_graph") == 0) {
        graph_display(&proc_graph);
    }

    else {
        printf("❌ Unknown command or invalid arguments.\n");
    }

    return 0;
}
