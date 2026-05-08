#include <kernel/core/mm/pmm.h>
#include <util/mem.h>
#include <stdint.h>
#include <kernel/core/vga/serial.h>

#define BITMAP_ADDR 0x00107000
#define BIT_SET(idx)            (bitmap[(idx) / 8] |= (1 << ((idx) % 8)))
#define BIT_CLEAR(idx)          (bitmap[(idx) / 8] &= ~(1 << ((idx) % 8)))
#define BIT_TEST(idx)           (bitmap[(idx) / 8] &   (1 << ((idx) % 8)))

static uint32_t BITMAP_SIZE;
static uint8_t* bitmap;

static void pmm_free_region(uint32_t base, uint32_t length);
void pmm_reserve_region(uint32_t base, uint32_t length);

void pmm_init(uint32_t msize, e820_entry_t* mmap, uint32_t mmap_count)
{
    BITMAP_SIZE = (msize / PAGE_SIZE) / 8;
    bitmap = (uint8_t*)BITMAP_ADDR;
    memset(bitmap, 0xFF, BITMAP_SIZE);

    serial_print("PMM: bitmap_size="); serial_print_hex(BITMAP_SIZE);
    serial_print("\nPMM: kernel_start="); serial_print_hex((uint32_t)&_kernel_start);
    serial_print("\nPMM: kernel_end="); serial_print_hex((uint32_t)&_kernel_end);
    serial_print("\n");

    for (uint32_t i = 0; i < mmap_count; i++)
    {
        if (mmap[i].type == 1)
        {
            serial_print("PMM: freeing "); serial_print_hex(mmap[i].base_low);
            serial_print(" len="); serial_print_hex(mmap[i].length_low);
            serial_print("\n");
            pmm_free_region(mmap[i].base_low, mmap[i].length_low);
        }
    }

    serial_print("PMM: reserving kernel\n");
    // pmm_reserve_region(0x0000, 0x1000);
    // pmm_reserve_region(0x1000, 0x8000);   // Stage2 lives here
    pmm_reserve_region(BITMAP_ADDR, 0x1000);  // at minimum one page
    pmm_reserve_region(0x0000, BITMAP_ADDR);
    pmm_reserve_region(BITMAP_ADDR, BITMAP_SIZE);
    pmm_reserve_region((uint32_t)&_kernel_start, (uint32_t)&_kernel_end - (uint32_t)&_kernel_start);
    serial_print("PMM: done\n");
}

static void pmm_free_region(uint32_t base, uint32_t length)
{
    uint32_t start_page = base / PAGE_SIZE;
    uint32_t end_page   = (base + length) / PAGE_SIZE;

    for (uint32_t i = start_page; i < end_page; i++)
    {
        BIT_CLEAR(i);
    }
}

void pmm_reserve_region(uint32_t base, uint32_t length)
{
    uint32_t start_page = base / PAGE_SIZE;
    uint32_t end_page   = (base + length + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = start_page; i < end_page; i++)
        BIT_SET(i);
}

uint32_t pmm_alloc()
{
    for (uint32_t i = 0; i < BITMAP_SIZE * 8; i++)
    {
        if (!BIT_TEST(i))
        {
            BIT_SET(i);
            return i * PAGE_SIZE;
        }
    }

    asm volatile("cli; hlt");
    return 0;   // out of memory
}

void pmm_free(uint32_t addr)
{
    BIT_CLEAR(addr / PAGE_SIZE);
}
