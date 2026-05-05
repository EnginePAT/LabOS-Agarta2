#include <userspace/core/syscall.h>
#include <kernel/core/vga/vga.h>

int exit(int code)
{
    vga_print("Process exited with code: ");
    vga_print("\n");

    __asm__ volatile (
        "int $0x80"
        :
        : "a"(0), "b"(code)
    );
    for (;;);
    return 0;
}

int write(int fd, const char *buf, unsigned int len)
{
    int ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(1), "b"(fd), "c"(buf), "d"(len)
    );
    return ret;
}
