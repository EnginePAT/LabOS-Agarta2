#include "LabFS-Lite.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: mkfs.labfs <image>\n");
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb+");
    if (!disk) {
        printf("mkfs.labfs: failed to open disk: '%s'\n", argv[1]);
        return -1;
    }

    // Read sector 0 (boot.bin is already stamped here)
    uint8_t sector0[512];
    fseek(disk, 0, SEEK_SET);
    fread(sector0, 512, 1, disk);

    // Superblock is embedded at offset +3 (after jmp short + nop)
    struct Superblock* sb = (struct Superblock*)(sector0 + 3);

    // Verify magic
    if (sb->magic != LABFS_MAGIC) {
        printf("mkfs.labfs: sector 0 does not contain a valid LabFS-Lite superblock!\n");
        printf("mkfs.labfs: did you forget to stamp boot.bin first?\n");
        fclose(disk);
        return -1;
    }

    // Reset inode_count to 1 (root inode only)
    sb->inode_count = 1;

    // Write sector 0 back with updated superblock
    fseek(disk, 0, SEEK_SET);
    fwrite(sector0, 512, 1, disk);

    // Sector 1: inode table - write root inode only
    struct Inode root_inode = {
        .type        = LABFS_TYPE_DIR,
        .size        = LABFS_BLOCK_SIZE,
        .start_block = sb->data_start,
        .block_count = 1
    };
    uint8_t inode_sector[512] = {0};
    memcpy(inode_sector, &root_inode, sizeof(struct Inode));
    fseek(disk, sb->inode_start * LABFS_BLOCK_SIZE, SEEK_SET);
    fwrite(inode_sector, 512, 1, disk);

    // Sector 2: empty directory entries
    uint8_t dir_sector[512] = {0};
    fseek(disk, sb->data_start * LABFS_BLOCK_SIZE, SEEK_SET);
    fwrite(dir_sector, 512, 1, disk);

    fclose(disk);

    printf("mkfs.labfs: initialized LabFS-Lite image '%s'\n", argv[1]);
    printf("  superblock  @ sector 0 (offset +3)\n");
    printf("  inode table @ sector %u\n", sb->inode_start);
    printf("  dir entries @ sector %u\n", sb->data_start);

    return 0;
}
