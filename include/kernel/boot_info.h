#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include <stdint.h>

/*
    LBootInfo
    - Data passed by the bootloader so the kernel knows important details
*/
struct LBootInfo {
    uint32_t magic;         // Bootloader magic number for verification
    uint32_t addr;          // Kernel address
    uint32_t memory_size;   // How much memory we actually have. This is set by the bootloader

    // Memory Map
    uint32_t mmap_count;
    uint32_t mmap_addr;
};

struct LFramebufferInfo {
    uint32_t framebuffer;   // Linear Framebuffer (LFB) address
    uint32_t width;
    uint32_t height;
    uint32_t pitch;         // Bytes per Row
    uint32_t bpp;           // Bits per Pixel
};

typedef struct e820_entry_t {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;      // 1 = usable, 2 = reserved, etc.
} __attribute__((packed)) e820_entry_t;

#endif      // BOOT_INFO_H
