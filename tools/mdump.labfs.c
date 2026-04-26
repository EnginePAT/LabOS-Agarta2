/*
    LabFS-Lite - mdump.labfs
    Dumps the filesystem structure in a human-readable format.
*/
#include "LabFS-Lite.h"
#include <stdio.h>
#include <string.h>

static int visited[LABFS_MAX_FILES] = {0};

static void print_hex(const uint8_t* data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (i % 16 == 0) printf("\n  %04zx: ", i);
        printf("%02x ", data[i]);
    }
    printf("\n");
}

static void walk_dir(
    FILE* disk,
    struct Superblock* sb,
    struct Inode* inodes,
    int inode_idx,
    const char* dirname,
    int depth)
{
    if (inode_idx < 0 || inode_idx >= LABFS_MAX_FILES)
        return;

    if (visited[inode_idx])
        return;
    visited[inode_idx] = 1;

    struct Inode* dir_inode = &inodes[inode_idx];

    // Print this directory
    for (int i = 0; i < depth; i++)
        printf("  ");

    if (depth == 0)
        printf("/\n");
    else
        printf("- %s/\n", dirname);

    // Load this directory's entries
    struct DirEntry entries[LABFS_MAX_FILES] = {0};

    if (dir_inode->block_count > 0) {
        fseek(disk, dir_inode->start_block * sb->block_size, SEEK_SET);
        fread(entries, sizeof(struct DirEntry), LABFS_MAX_FILES, disk);
    }

    // Walk entries
    for (int i = 0; i < LABFS_MAX_FILES; i++) {
        if (entries[i].name[0] == 0)
            continue;

        int child_idx = entries[i].inode_idx;
        if (child_idx < 0 || child_idx >= LABFS_MAX_FILES)
            continue;

        struct Inode* node = &inodes[child_idx];

        if (node->type == LABFS_TYPE_DIR) {
            walk_dir(disk, sb, inodes, child_idx, entries[i].name, depth + 1);
        }
        else if (node->type == LABFS_TYPE_FILE) {
            for (int d = 0; d < depth + 1; d++)
                printf("  ");
            printf("- %s\n", entries[i].name);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: mdump <image>\n");
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");
    if (!disk) {
        printf("mdump: error: failed to open disk '%s'\n", argv[1]);
        return -1;
    }

    // Read sector 0 (boot sector)
    uint8_t sector0[512] = {0};
    fread(sector0, 512, 1, disk);

    struct Superblock sb = (struct Superblock){0};
    memcpy(&sb, sector0 + 3, sizeof(struct Superblock));

    printf("=== LabFS-Lite Dump ===\n");
    printf("Boot sector (first 32 bytes):");
    print_hex(sector0, 32);

    if (sb.magic != LABFS_MAGIC) {
        printf("\nERROR: Not a LabFS-Lite filesystem (magic mismatch)\n");
        fclose(disk);
        return -1;
    }

    printf("\nSuperblock @ sector 0 (+3 offset):\n");
    printf("  Magic:        0x%04x\n", sb.magic);
    printf("  Version:      %u\n", sb.version);
    printf("  Block size:   %u\n", sb.block_size);
    printf("  Inode start:  %u\n", sb.inode_start);
    printf("  Data start:   %u\n", sb.data_start);
    printf("  Inode count:  %u\n", sb.inode_count);
    printf("  Root inode:   %u\n", sb.root_inode);

    // Read inode table
    struct Inode inodes[LABFS_MAX_FILES] = {0};
    fseek(disk, sb.inode_start * LABFS_BLOCK_SIZE, SEEK_SET);
    fread(inodes, sizeof(struct Inode), LABFS_MAX_FILES, disk);

    printf("\nInode Table @ sector %u:\n", sb.inode_start);
    for (uint32_t i = 0; i < sb.inode_count; i++) {
        printf("  Inode %u:\n", i);
        printf("    Type:        %u (%s)\n",
            inodes[i].type,
            (inodes[i].type == LABFS_TYPE_DIR ? "DIR" :
             inodes[i].type == LABFS_TYPE_FILE ? "FILE" : "UNKNOWN"));
        printf("    Size:        %u bytes\n", inodes[i].size);
        printf("    Start block: %u\n", inodes[i].start_block);
        printf("    Block count: %u\n", inodes[i].block_count);
    }

    // Read directory entries
    struct DirEntry entries[LABFS_MAX_FILES] = {0};
    fseek(disk, sb.data_start * LABFS_BLOCK_SIZE, SEEK_SET);
    fread(entries, sizeof(struct DirEntry), LABFS_MAX_FILES, disk);

    printf("\nDirectory Entries @ sector %u:\n", sb.data_start);
    for (int i = 0; i < LABFS_MAX_FILES; i++) {
        if (entries[i].name[0] == 0)
            continue;

        printf("  Entry %d:\n", i);
        printf("    Name:      %s\n", entries[i].name);
        printf("    Inode idx: %u\n", entries[i].inode_idx);
    }

    printf("\nFilesystem Tree:\n");
    walk_dir(disk, &sb, inodes, sb.root_inode, "/", 0);

    printf("\n=== End of Dump ===\n");

    fclose(disk);
    return 0;
}
