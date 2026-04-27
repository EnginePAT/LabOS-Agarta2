#include <kernel/disk/ata.h>

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
    {
        ((uint16_t*)buffer)[i] = inw(0x1F0);
    }
}
