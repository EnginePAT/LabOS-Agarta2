/*
    LabFS-Lite
    Copyright 2026 EnginePAT. All rights resvered.
    Licensed under the Apache 2.0 license. See LICENSE for more details.

    LabFS-Lite is a filesystem specifically designed for BIOS bootloaders.
    Currently supports 1 directory, and up to 3 files.

    Usage: mcopy <image> <host_file> <fs_path>
    Example: mcopy disk.img build/stage2.bin /stage2.bin
*/
#include "LabFS-Lite.h"
#include <stdio.h>
#include <string.h>

static int split_path(const char* path, char parts[][LABFS_MAX_FILENAME], int max)
{
    int count = 0;
    const char* p = path;

    if (*p == '/') p++;

    while (*p && count < max) {
        char* out = parts[count];
        int len = 0;

        while (*p && *p != '/' && len < LABFS_MAX_FILENAME - 1)
            out[len++] = *p++;

        out[len] = 0;
        count++;

        if (*p == '/') p++;
    }

    return count;
}

static int find_in_dir(FILE* disk, struct Superblock* sb,
                       struct Inode* inodes, int dir_inode_idx,
                       const char* name)
{
    struct Inode* dir = &inodes[dir_inode_idx];
    struct DirEntry entries[LABFS_MAX_FILES] = {0};

    if (dir->block_count == 0)
        return -1;

    fseek(disk, dir->start_block * sb->block_size, SEEK_SET);
    fread(entries, sizeof(struct DirEntry), LABFS_MAX_FILES, disk);

    for (int i = 0; i < LABFS_MAX_FILES; i++) {
        if (entries[i].name[0] == 0)
            continue;
        if (strcmp(entries[i].name, name) == 0)
            return entries[i].inode_idx;
    }

    return -1;
}

static int add_to_dir(FILE* disk, struct Superblock* sb,
                      struct Inode* inodes, int dir_inode_idx,
                      int new_inode_idx, const char* name)
{
    struct Inode* dir = &inodes[dir_inode_idx];
    struct DirEntry entries[LABFS_MAX_FILES] = {0};

    if (dir->block_count == 0)
        return -1;

    fseek(disk, dir->start_block * sb->block_size, SEEK_SET);
    fread(entries, sizeof(struct DirEntry), LABFS_MAX_FILES, disk);

    for (int i = 0; i < LABFS_MAX_FILES; i++) {
        if (entries[i].name[0] == 0) {
            strncpy(entries[i].name, name, LABFS_MAX_FILENAME);
            entries[i].inode_idx = new_inode_idx;

            fseek(disk, dir->start_block * sb->block_size, SEEK_SET);
            fwrite(entries, sizeof(struct DirEntry), LABFS_MAX_FILES, disk);
            return 0;
        }
    }

    return -1;
}

int main(int argc, char* argv[])
{
    if (argc != 4) return -1;

    FILE* disk = fopen(argv[1], "rb+");
    if (!disk) {
        printf("mcopy.labfs: error: failed to open disk '%s'\n", argv[1]);
        return -1;
    }

    uint8_t sector0[512] = {0};
    fread(sector0, 512, 1, disk);

    struct Superblock sb = {0};
    memcpy(&sb, sector0 + 3, sizeof(struct Superblock));

    if (sb.magic != LABFS_MAGIC) {
        printf("mcopy.labfs: error: not a LabFS-Lite disk\n");
        fclose(disk);
        return -1;
    }

    struct Inode inodes[LABFS_MAX_FILES] = {0};
    fseek(disk, sb.inode_start * sb.block_size, SEEK_SET);
    fread(inodes, sizeof(struct Inode), LABFS_MAX_FILES, disk);

    // Parse path
    char parts[16][LABFS_MAX_FILENAME] = {0};
    int count = split_path(argv[3], parts, 16);

    if (count == 0) {
        printf("mcopy.labfs: error: invalid path '%s'\n", argv[3]);
        fclose(disk);
        return -1;
    }

    // Walk directories
    int cur = sb.root_inode;

    for (int i = 0; i < count - 1; i++) {
        int next = find_in_dir(disk, &sb, inodes, cur, parts[i]);
        if (next < 0) {
            printf("mcopy.labfs: error: directory '%s' not found\n", parts[i]);
            fclose(disk);
            return -1;
        }
        cur = next;
    }

    const char* filename = parts[count - 1];

    // Find next free inode
    int new_inode = sb.inode_count;
    if (new_inode >= LABFS_MAX_FILES) {
        printf("mcopy.labfs: error: inode table full\n");
        fclose(disk);
        return -1;
    }

    // Find next free block (respect both files and directories)
    uint32_t next_block = sb.data_start;

    for (int i = 0; i < sb.inode_count; i++) {
        if (inodes[i].block_count == 0)
            continue;

        uint32_t end = inodes[i].start_block + inodes[i].block_count;
        if (end > next_block)
            next_block = end;
    }

    // Open source file
    FILE* src = fopen(argv[2], "rb");
    if (!src) {
        printf("mcopy.labfs: error: cannot open '%s'\n", argv[2]);
        fclose(disk);
        return -1;
    }

    fseek(src, 0, SEEK_END);
    long size = ftell(src);
    rewind(src);

    uint32_t blocks = (size + 511) / 512;

    printf("mcopy.labfs: copying '%s' -> '%s' (%ld bytes, %u blocks)\n",
           argv[2], argv[3], size, blocks);

    // Write file data
    fseek(disk, next_block * sb.block_size, SEEK_SET);
    char buf[512] = {0};
    size_t n;
    while ((n = fread(buf, 1, 512, src)) > 0) {
        fwrite(buf, 1, 512, disk);
        memset(buf, 0, 512);
    }
    fclose(src);

    // Create inode
    inodes[new_inode].type = LABFS_TYPE_FILE;
    inodes[new_inode].size = size;
    inodes[new_inode].start_block = next_block;
    inodes[new_inode].block_count = blocks;

    // Insert into directory
    if (add_to_dir(disk, &sb, inodes, cur, new_inode, filename) < 0) {
        printf("mcopy.labfs: error: directory full\n");
        fclose(disk);
        return -1;
    }

    // Write inode table
    fseek(disk, sb.inode_start * sb.block_size, SEEK_SET);
    fwrite(inodes, sizeof(struct Inode), LABFS_MAX_FILES, disk);

    // Update superblock
    sb.inode_count++;
    memcpy(sector0 + 3, &sb, sizeof(struct Superblock));

    fseek(disk, 0, SEEK_SET);
    fwrite(sector0, 512, 1, disk);

    printf("mcopy.labfs: done!\n");
    fclose(disk);
    return 0;
}
