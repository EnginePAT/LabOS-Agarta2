#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 16
#define KERNEL_STACK_SIZE 4096

typedef enum {
    P_READY,
    P_RUNNING,
    P_BLOCKED,
    P_DEAD
} process_state_t;

typedef struct {
    uint32_t pid;
    uint32_t esp;                   // Saved kernel stack pointer
    uint32_t* page_dir;             // Physical address of page directory
    uint32_t kernel_stack;          // Top of this process's kernel stack
    process_state_t state;
} process_t;

extern process_t process_table[MAX_PROCESSES];
extern process_t* current_process;

void process_init();
process_t* process_create(uint32_t entry, uint32_t* page_dir);
void process_exit(uint32_t pid);

#endif      // PROCESS_H
