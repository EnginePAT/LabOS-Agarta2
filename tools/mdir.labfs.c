/*
    LabFS-Lite - mdir.labfs
    Creates a directory with full path support.
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
    if (argc != 4) {
        printf("Usage: mdir <image> <dir_name> <fs_path>\n");
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb+");
    if (!disk) {
        printf("mdir: error: failed to open disk '%s'\n", argv[1]);
        return -1;
    }

    uint8_t sector0[512] = {0};
    fread(sector0, 512, 1, disk);

    struct Superblock sb = {0};
    memcpy(&sb, sector0 + 3, sizeof(struct Superblock));

    if (sb.magic != LABFS_MAGIC) {
        printf("mdir: error: not a LabFS-Lite image\n");
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
        printf("mdir: error: invalid path '%s'\n", argv[3]);
        fclose(disk);
        return -1;
    }

    // Walk to parent directory
    int cur = sb.root_inode;

    for (int i = 0; i < count - 1; i++) {
        int next = find_in_dir(disk, &sb, inodes, cur, parts[i]);
        if (next < 0) {
            printf("mdir: error: directory '%s' not found\n", parts[i]);
            fclose(disk);
            return -1;
        }
        cur = next;
    }

    const char* new_dir_name = parts[count - 1];

    // Check if directory already exists
    if (find_in_dir(disk, &sb, inodes, cur, new_dir_name) >= 0) {
        printf("mdir: directory '%s' already exists\n", new_dir_name);
        fclose(disk);
        return 0;
    }

    // Allocate inode
    int new_inode = sb.inode_count;
    if (new_inode >= LABFS_MAX_FILES) {
        printf("mdir: error: inode table full\n");
        fclose(disk);
        return -1;
    }

    // Allocate block for directory
    uint32_t next_block = sb.data_start + 1;
    for (int i = 0; i < sb.inode_count; i++) {
        uint32_t end = inodes[i].start_block + inodes[i].block_count;
        if (end > next_block)
            next_block = end;
    }

    // Write empty directory block
    struct DirEntry empty[LABFS_MAX_FILES] = {0};
    fseek(disk, next_block * sb.block_size, SEEK_SET);
    fwrite(empty, sizeof(struct DirEntry), LABFS_MAX_FILES, disk);

    // Create inode
    inodes[new_inode].type = LABFS_TYPE_DIR;
    inodes[new_inode].size = sb.block_size;
    inodes[new_inode].start_block = next_block;
    inodes[new_inode].block_count = 1;

    // Insert into parent directory
    if (add_to_dir(disk, &sb, inodes, cur, new_inode, new_dir_name) < 0) {
        printf("mdir: error: parent directory full\n");
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

    printf("mdir: created directory '%s'\n", new_dir_name);
    fclose(disk);
    return 0;
}
