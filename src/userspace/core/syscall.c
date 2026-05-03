#include <userspace/core/syscall.h>
#include <kernel/core/vga/vga.h>

int exit(int code)
{
    vga_print("Process exited with code: ");
    vga_print((char*)code);
    vga_print("\n");

    // For now, just halt the system
    // Later this will call the scheduler to kill the process
    for (;;);
    return 0;
}
