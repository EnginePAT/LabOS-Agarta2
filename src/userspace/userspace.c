#include <userspace/userspace.h>
#include <kernel/core/mm/gdt.h>

extern void jump_usermode();

void userspace_init()
{
    // Set kernel stack in TSS
    extern struct tss_entry_struct tss_entry;
    tss_entry.esp0 = 0x90000;       // Kernel stack
    tss_entry.ss0 = 0x10;           // Kernel data segment

    // Call our assembly function which jumps to Ring 3
    jump_usermode();
}
