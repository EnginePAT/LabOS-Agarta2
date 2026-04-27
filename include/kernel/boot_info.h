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
};

struct LFramebufferInfo {
    uint32_t framebuffer;   // Linear Framebuffer (LFB) address
    uint32_t width;
    uint32_t height;
    uint32_t pitch;         // Bytes per Row
    uint32_t bpp;           // Bits per Pixel
};

#endif      // BOOT_INFO_H
