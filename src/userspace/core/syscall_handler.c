#include <kernel/core/vga/vga.h>
#include <stdint.h>
#include <userspace/core/syscall_handler.h>
#include <kernel/registers.h>

typedef int (*syscall_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

static syscall_t syscall_table[] = {};

void syscall_handler(registers_t* regs)
{
    // The syscall number is in EAX
    uint32_t syscall_num = regs->eax;

    if (syscall_num >= (sizeof(syscall_table) / sizeof(syscall_t)) || syscall_table[syscall_num] == NULL) {
        regs->eax = -1;         // Return an error if the syscall doesn't exist
        return;
    }

    // Call the function from the table
    // Passing the GP registers as arguments
    syscall_t handler = syscall_table[syscall_num];
    regs->eax = handler(regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
}

void sys_exit(int code)
{
    vga_print("Process exited with code: ");
    vga_print((char*)code);
    vga_print("\n");

    __asm__ volatile (
        "int $0x80"
        :
        : "a"(0), "b"(code)         // 'a' is EAX (syscall 0), 'b' is EBX (exit code)
    );
}
