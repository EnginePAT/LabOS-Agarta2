/*
 * LabOS Agarta
 * Copyright (c) 2026 EnginePAT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "kernel/core/vga/serial.h"
#include "util/util.h"
#include <kernel/core/vga/vga.h>
#include <stdint.h>
#include <userspace/core/syscall_handler.h>
#include <userspace/core/handlers/handlers.h>

typedef int (*syscall_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);


static syscall_t syscall_table[] = {
    sys_exit_handler,
    sys_write_handler,
};

void syscall_handler(struct InterruptRegisters* regs)
{
    serial_print("SYSCALL: eax=");
    serial_print_hex(regs->eax);
    serial_print("\n");
    
    // The syscall number is in EAX
    uint32_t syscall_num = regs->eax;

    if (syscall_num >= (sizeof(syscall_table) / sizeof(syscall_t)) || syscall_table[syscall_num] == NULL) {
        regs->eax = -1;         // Return an error if the syscall doesn't exist
        return;
    }

    if (syscall_num == 1) {
        serial_print((char*)regs->ecx);
    }

    // Call the function from the table
    // Passing the GP registers as arguments
    syscall_t handler = syscall_table[syscall_num];
    regs->eax = handler(regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
}

void sys_exit(int code)
{
    vga_print("Process exited with code: ");
    vga_print_hex(code);
    vga_print("\n");

    __asm__ volatile (
        "int $0x80"
        :
        : "a"(0), "b"(code)         // 'a' is EAX (syscall 0), 'b' is EBX (exit code)
    );
}
