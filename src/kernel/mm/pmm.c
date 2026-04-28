#include <kernel/mm/pmm.h>
#include <util/mem.h>
#include <stdint.h>

#define BITMAP_ADDR 0x10000

static uint32_t BITMAP_SIZE;
static uint8_t* bitmap;

void pmm_init(uint32_t msize)
{
    // Bitmap Size = (memory size / sizeof page) / 8 bpp
    BITMAP_SIZE = (msize / PAGE_SIZE) / 8;
    bitmap = (uint8_t*)BITMAP_ADDR;
    memset(bitmap, 0xFF, BITMAP_SIZE);      // Mark everything used

    // TODO: Mark free regions from the memory map
}
