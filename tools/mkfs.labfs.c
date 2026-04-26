#include "LabFS-Lite.h"
#include <string.h>
#include <stdio.h>

static void write_padded(FILE* disk, void* data, size_t data_size, size_t total_size)
{
    fwrite(data, data_size, 1, disk);
    size_t remaining = total_size - data_size;
    if (remaining > 0) {
        char zeroes[512] = {0};
        fwrite(zeroes, remaining, 1, disk);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) return -1;

    // Get actual stage2.bin size first
    FILE* f = fopen("build/stage2.bin", "rb");
    if (!f) { printf("mkfs.labfs: could not open build/stage2.bin\n"); return -1; }
    fseek(f, 0, SEEK_END);
    long stage2_size = ftell(f);
    rewind(f);
    uint32_t stage2_sectors = (stage2_size + 511) / 512;

    printf("mkfs.labfs: stage2.bin is %ld bytes (%u sectors)\n", stage2_size, stage2_sectors);

    struct Superblock sb = {
        .magic      = LABFS_MAGIC,
        .version    = LABFS_VERSION,
        .resv       = 0,
        .block_size = LABFS_BLOCK_SIZE,
        .inode_start = 1,
        .data_start  = 2,
        .inode_count = 2,
        .root_inode  = 0,
        .reserved    = {0, 0}
    };

    struct Inode root_inode = {
        .type        = LABFS_TYPE_DIR,
        .size        = LABFS_BLOCK_SIZE,
        .start_block = sb.data_start,
        .block_count = 1
    };

    struct Inode stage2_inode = {
        .type        = LABFS_TYPE_FILE,
        .size        = (uint32_t)stage2_size,
        .start_block = 3,                  // sector 3 onwards
        .block_count = stage2_sectors
    };

    struct DirEntry ent = {0};
    strncpy(ent.name, "stage2.bin", LABFS_MAX_FILENAME);
    ent.inode_idx = 1;

    FILE* disk = fopen(argv[1], "wb+");
    if (!disk) {
        printf("mkfs.labfs: failed to open disk: '%s'\n", argv[1]);
        fclose(f);
        return -1;
    }

    // Sector 0: superblock (padded to 512 bytes)
    fseek(disk, 0, SEEK_SET);
    write_padded(disk, &sb, sizeof(struct Superblock), LABFS_BLOCK_SIZE);

    // Sector 1: inode table (root inode + stage2 inode, padded to 512 bytes)
    fseek(disk, sb.inode_start * LABFS_BLOCK_SIZE, SEEK_SET);
    fwrite(&root_inode,   sizeof(struct Inode), 1, disk);
    fwrite(&stage2_inode, sizeof(struct Inode), 1, disk);
    // Pad rest of sector
    size_t inode_bytes = sizeof(struct Inode) * 2;
    char zeroes[512] = {0};
    fwrite(zeroes, LABFS_BLOCK_SIZE - inode_bytes, 1, disk);

    // Sector 2: directory entries (padded to 512 bytes)
    fseek(disk, sb.data_start * LABFS_BLOCK_SIZE, SEEK_SET);
    write_padded(disk, &ent, sizeof(struct DirEntry), LABFS_BLOCK_SIZE);

    // Sector 3+: stage2.bin raw data
    fseek(disk, stage2_inode.start_block * LABFS_BLOCK_SIZE, SEEK_SET);
    char buf[512];
    size_t n;
    while ((n = fread(buf, 1, 512, f)) > 0)
        fwrite(buf, 1, n, disk);

    fclose(f);
    fclose(disk);

    printf("mkfs.labfs: disk image written successfully!\n");
    printf("  superblock  @ sector 0\n");
    printf("  inode table @ sector %u\n", sb.inode_start);
    printf("  dir entries @ sector %u\n", sb.data_start);
    printf("  stage2.bin  @ sector %u (%u sectors)\n", stage2_inode.start_block, stage2_sectors);

    return 0;
}
