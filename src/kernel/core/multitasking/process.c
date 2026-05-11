#include <kernel/core/multitasking/process.h>
#include <kernel/core/mm/memory.h>
#include <kernel/core/mm/vmm.h>
#include <stdint.h>

process_t process_table[MAX_PROCESSES];
process_t* current_process;

void process_init()
{
    // Zero out the process table
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i].state = P_DEAD;
    }

    // Create a process for the kernel (PID 0)
    process_t* kproc = &process_table[0];
    kproc->pid = 0;
    kproc->state = P_RUNNING;
    kproc->page_dir = current_dir;
    kproc->esp = 0;
    kproc->kernel_stack = 0;

    current_process = kproc;
}

process_t* process_create(uint32_t entry, uint32_t* page_dir)
{
    process_t* proc = 0;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].state == P_DEAD)
        {
            // This is free to use!
            proc = &process_table[i];
            proc->pid = i;
            break;
        }
    }

    if (!proc) return 0;

    // Allocate space for stack
    uint8_t* stack = (uint8_t*)kmalloc(KERNEL_STACK_SIZE);
    if (!stack) return 0;

    proc->kernel_stack = (uint32_t)stack + KERNEL_STACK_SIZE;
    uint32_t* stk = (uint32_t*)(proc->kernel_stack & ~0x3);
    *--stk = entry;         // Return will pop this as EIP
    *--stk = 0;             // EBP
    *--stk = 0;             // EBX
    *--stk = 0;             // ESI
    *--stk = 0;             // EDI

    proc->esp = (uint32_t)stk;
    proc->state = P_READY;
    proc->page_dir = page_dir;
    return proc;
}
