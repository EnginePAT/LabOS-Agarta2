#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

#define MACH4_MAGIC 0x4D344558
#define MACH4_MAGIC_LEN 4
#define MACH4_VERSION 1

struct mach4_header_t {
    uint32_t magic;               // "M4EX"
    uint8_t version;                // 1
    uint8_t flags;                  // Reserved for now

    uint32_t entry_offset;          // Where the _start label is in our code. Can be changed if needed
    uint32_t fsize;                 // How big the file is
    uint32_t permissions;           // Is it admin-only writeable, can the user run it?
    uint16_t owner;                 // Who owns this?
}__attribute__((packed));

uint32_t mach4_load(const char* path, uint32_t addr);
void mach4_execute(uint32_t id);

#endif      // LOADER_H
