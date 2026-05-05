#include "userspace/core/syscall.h"
#include <kernel/core/vga/vga.h>
#include <userspace/userspace.h>
#include <kernel/core/mm/gdt.h>
#include <userspace/core/syscall_handler.h>

extern void jump_usermode();

void userspace_init()
{
    // Set kernel stack in TSS
    extern struct tss_entry_struct tss_entry;
    tss_entry.esp0 = 0x90000;             // Kernel stack
    tss_entry.ss0 = 0x10;                 // Kernel data segment

    vga_print("Welcome to Userland!\n");

    // Call our assembly function which jumps to Ring 3
    jump_usermode();

    sys_exit(0);
    write(1, "Hello, world from userland!", 30);
}
