#ifndef ATA_H
#define ATA_H

#include <stdint.h>

// Read an ATA drive into a buffer stored in memory
void ata_read28(uint32_t lba, uint8_t* buffer, int slave);

#endif      // ATA_H
