#include <userspace/core/syscall.h>


extern void main() {
    write(1, "Hello, world from userland!", 30);
    // int x = 1 / 0;          // We can't do this without getting an infinite result - should trigger a fault
    for (;;);
}
