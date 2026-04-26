#include "fs/labfs.h"
#include <stdint.h>

volatile uint8_t* video = (uint8_t*)0xb8000;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void ata_read28(uint32_t lba, uint8_t* buffer, int slave) {
    while (inb(0x1F7) & 0x80);

    // bit 4 = 0 for master, 1 for slave
    outb(0x1F6, (slave ? 0xF0 : 0xE0) | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)(lba));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    while (!(inb(0x1F7) & 0x08));

    for (int i = 0; i < 256; i++)
        ((uint16_t*)buffer)[i] = inw(0x1F0);
}

void print(const char* s)
{
    for (int i = 0; s[i] != '\0'; i++)
    {
        // Print a character to the screen located at s[i]
        video[i * 2] = s[i];
        video[i * 2 + 1] = 0x0f;
    }
}

static inline void serial_out(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0);  // Wait for TX ready
    outb(0x3F8, c);
}

void serial_print(const char* s) {
    while (*s) serial_out(*s++);
}

static void serial_init(void) {
    outb(0x3F8 + 1, 0x00); // disable interrupts
    outb(0x3F8 + 3, 0x80); // enable DLAB
    outb(0x3F8 + 0, 0x03); // divisor low  (38400 baud)
    outb(0x3F8 + 1, 0x00); // divisor high
    outb(0x3F8 + 3, 0x03); // 8N1, clear DLAB
    outb(0x3F8 + 2, 0xC7); // enable FIFO, clear, 14-byte threshold
    outb(0x3F8 + 4, 0x0B); // IRQs off, RTS/DSR set
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

extern void stage2_main(unsigned int magic, unsigned int addr)
{
    serial_init();
    serial_print("Stage2 started\r\n");
    print("Hello, world!");

    uint8_t buffer[512];
    ata_read28(0, buffer, 1);
    struct Superblock* sb = (struct Superblock*)buffer;

    // Dump the contents to the serial console
    serial_print("Magic:       "); serial_print_hex(sb->magic);       serial_print("\r\n");
    serial_print("Version:     "); serial_print_hex(sb->version);     serial_print("\r\n");
    serial_print("Inode start: "); serial_print_hex(sb->inode_start); serial_print("\r\n");
    serial_print("Data start:  "); serial_print_hex(sb->data_start);  serial_print("\r\n");

    while (1);
}
