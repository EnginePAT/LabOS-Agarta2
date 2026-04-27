#include <kernel/core/shell.h>
#include <kernel/core/keyboard.h>
#include <kernel/core/vga/vga.h>
#include <util/util.h>

void shell_main()
{
    vga_print("Welcome to LabOS Agarta!\n\n");
    vga_print("[Agarta]: ~$ ");

    while (1)
    {
        asm volatile ("hlt");       // Wait for IRQ1 to fire

        if (command_ready)
        {
            char* cmd = keyboard_get_command();
            if (strcmp(cmd, "Hello") == 0)
            {
                vga_print("Hello, world!\n");
            }

            // More commands as necessary

            vga_print("[Agarta]: ~$ ");
            command_ready = 0;      // Clear the flag
        }
    }
}
