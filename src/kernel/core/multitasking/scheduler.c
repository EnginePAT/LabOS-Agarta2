#include <kernel/core/multitasking/scheduler.h>
#include <kernel/core/multitasking/process.h>
#include <kernel/core/vga/serial.h>
#include <stdint.h>

void schedule(struct InterruptRegisters* regs)
{
    if (!current_process) return;

    process_t* next = pick_next_process();
    if (!next || next == current_process) return;

    process_t* prev = current_process;
    current_process = next;
    next->state = P_RUNNING;
    prev->state = P_READY;

    context_switch(&prev->esp, next->esp, (uint32_t)next->page_dir);
}

process_t* pick_next_process()
{
    uint32_t start = current_process->pid;
    uint32_t i = (start + 1) % MAX_PROCESSES;

    while (i != start)
    {
        if (process_table[i].state == P_READY)
        {
            return &process_table[i];
        }
        
        i = (i + 1) % MAX_PROCESSES;
    }
    
    return current_process;                 // No other processes, keep running
}
