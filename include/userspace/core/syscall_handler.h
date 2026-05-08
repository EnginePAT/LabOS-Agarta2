#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include "util/util.h"
#include <kernel/registers.h>

void sys_exit(int code);
void syscall_handler(struct InterruptRegisters* regs);

#endif          // SYSCALL_HANDLER_H
