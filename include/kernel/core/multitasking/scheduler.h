#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <kernel/core/multitasking/process.h>
#include <util/util.h>

extern void context_switch(uint32_t* old_esp, uint32_t new_esp, uint32_t new_cr3);

void scheduler_init();
void schedule(struct InterruptRegisters* regs);
process_t* pick_next_process();

#endif          // SCHEDULER_H
