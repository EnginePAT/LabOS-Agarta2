#include <kernel/core/mm/gdt.h>
#include <util/util.h>
#include <util/mem.h>


extern void gdt_flush(uint32_t);
extern void tss_flush();

struct gdt_entry_struct gdt_entries[6];
struct gdt_ptr_struct gdt_ptr;
struct tss_entry_struct tss_entry;

void initGdt()
{
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    setGdtGate(0,0,0,0,0); //Null segment
    setGdtGate(1,0,0xFFFFFFFF, 0x9A, 0xCF);     // Kernel code segment
    setGdtGate(2,0,0xFFFFFFFF, 0x92, 0xCF);     // Kernel data segment
    setGdtGate(3,0,0xFFFFFFFF, 0xFA, 0xCF);     // User code segment
    setGdtGate(4,0,0xFFFFFFFF, 0xF2, 0xCF);     // User data segment

    writeTSS(5,0x10, 0x0);

    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
}

void writeTSS(uint32_t num, uint16_t ss0, uint32_t esp0)
{
    uint32_t base  = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    // Correct TSS descriptor type
    setGdtGate(num, base, limit, 0x89, 0x00);

    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0  = ss0;   // kernel data selector (0x10)
    tss_entry.esp0 = esp0;

    // These are only used on hardware task switch
    tss_entry.cs = 0x08;    // kernel code
    tss_entry.ss = 0x10;    // kernel data
    tss_entry.ds = 0x10;
    tss_entry.es = 0x10;
    tss_entry.fs = 0x10;
    tss_entry.gs = 0x10;
}

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{

    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit = (limit & 0xFFFF);
    gdt_entries[num].flags = (limit >> 16) & 0x0F;
    gdt_entries[num].flags |= (gran & 0xF0);

    gdt_entries[num].access = access;

}
