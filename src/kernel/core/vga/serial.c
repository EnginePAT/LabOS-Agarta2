#include <kernel/core/vga/serial.h>
#include <util/util.h>

void serial_init(void) {
    outb(0x3F8 + 1, 0x00); // disable interrupts
    outb(0x3F8 + 3, 0x80); // enable DLAB
    outb(0x3F8 + 0, 0x03); // divisor low  (38400 baud)
    outb(0x3F8 + 1, 0x00); // divisor high
    outb(0x3F8 + 3, 0x03); // 8N1, clear DLAB
    outb(0x3F8 + 2, 0xC7); // enable FIFO, clear, 14-byte threshold
    outb(0x3F8 + 4, 0x0B); // IRQs off, RTS/DSR set
}

void serial_out(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0);  // Wait for TX ready
    outb(0x3F8, c);
}

void serial_print(const char* s) {
    while (*s) serial_out(*s++);
}

void serial_hex_dump(uint8_t* buf, int len) {
    char hex[] = "0123456789ABCDEF";
    for (int i = 0; i < len; i++) {
        serial_out(hex[buf[i] >> 4]);
        serial_out(hex[buf[i] & 0xF]);
        serial_out(' ');
        if ((i + 1) % 16 == 0) serial_print("\r\n");
    }
}

void serial_print_hex(uint32_t val) {
    char hex[] = "0123456789ABCDEF";
    serial_print("0x");
    for (int i = 28; i >= 0; i -= 4)
        serial_out(hex[(val >> i) & 0xF]);
}
