/*
 * LabOS Agarta
 * Copyright (c) 2026 EnginePAT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <kernel/fs/ext2.h>
#include <kernel/disk/ata.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/vga/serial.h>
#include <util/util.h>
#include <util/mem.h>
#include <stdint.h>

#define EXT2_DISK_DRIVE 1                   // Master 8, drive 1 (0x81)

struct ext2_super_block sb;
static struct ext2_block_group_desc bgd;
static uint32_t block_size;
static uint32_t sectors_per_block;

void ext2_init()
{
    // Read superblock
    // NOTE: The superblock is always at byte 1024 (sector 2)
    uint8_t sb_buf[1024];
    ata_read28(2, sb_buf, EXT2_DISK_DRIVE);       // first 512 bytes
    ata_read28(3, sb_buf + 512, EXT2_DISK_DRIVE); // second 512 bytes

    // Copy superblock (can't use pointer directly, stack gets clobbered)
    kmemcpy((uint8_t*)&sb, sb_buf, (int)sizeof(struct ext2_super_block));

    // Verify the superblock to ensure the filesystem is indeed EXT2
    if (sb.s_magic != EXT2_MAGIC)
    {
        vga_print("ext2: invalid magic!\n");
        return;
    }

    serial_print("sb_buf[0x38..0x48]: ");
    for (int i = 0x38; i < 0x48; i++) {
        serial_print_hex(sb_buf[i]);
        serial_print(" ");
    }
    serial_print("\r\n");

    block_size = EXT2_BLOCK_SIZE(&sb);
    sectors_per_block = block_size / 512;           // 512 bytes per sector

    serial_print("ext2: block size = ");
    serial_print_hex(block_size);
    serial_print("\r\n");

    // Read block group descriptor table
    // Always in block 2 for 1KB blocks, block 1 otherwise
    uint32_t bgdt_block = (block_size == 1024) ? 2 : 1;

    uint8_t bgdt_buf[1024];
    uint32_t sector = bgdt_block * sectors_per_block;

    ata_read28(sector, bgdt_buf, EXT2_DISK_DRIVE);
    ata_read28(sector + 1, bgdt_buf + 512, EXT2_DISK_DRIVE);

    kmemcpy(&bgd, bgdt_buf, sizeof(bgd));

    vga_print("EXT2 mounted!\n");
}

struct ext2_inode_table read_inode(uint32_t inode_num)
{
    struct ext2_inode_table inode_t;

    uint32_t inode_idx = inode_num - 1;
    uint32_t inodes_per_block = EXT2_BLOCK_SIZE(&sb) / sb.s_inode_size;
    uint32_t block_offset = inode_idx / inodes_per_block;
    uint32_t inode_offset = inode_idx % inodes_per_block;

    uint32_t sector = (bgd.bg_inode_table + block_offset) * sectors_per_block;
    uint32_t byte_offset = inode_offset * sb.s_inode_size;

    uint8_t buf[1024];
    ata_read28(sector,     buf,       EXT2_DISK_DRIVE);
    ata_read28(sector + 1, buf + 512, EXT2_DISK_DRIVE);

    kmemcpy((uint8_t*)&inode_t, buf + byte_offset, sizeof(struct ext2_inode_table));
    // ABI: src = buf + byte_offset, dest = &inode_t

    return inode_t;
}

uint32_t ext2_find_entry(uint32_t dir_inode, const char* name)
{
    struct ext2_inode_table inode_t = read_inode(dir_inode);
    uint32_t* blocks = (uint32_t*)inode_t.i_block;
    uint32_t data_block = blocks[0];

    uint8_t buf[1024];
    uint32_t sector = data_block * sectors_per_block;

    ata_read28(sector,     buf,       EXT2_DISK_DRIVE);
    ata_read28(sector + 1, buf + 512, EXT2_DISK_DRIVE);

    int pos = 0;
    while (pos < block_size)
    {
        struct ext2_dir_entry* entry = (struct ext2_dir_entry*)(buf + pos);
        if (entry->rec_len == 0) break;

        if (entry->inode != 0)
        {
            char ename[256];
            kmemcpy(ename, entry->name, entry->name_len);  // src = entry->name, dest = ename
            ename[entry->name_len] = '\0';

            if (strcmp(ename, name) == 0)
                return entry->inode;
        }
        pos += entry->rec_len;
    }
    return 0;       // Not found
}

void read_entries(uint32_t inode_num)
{
    struct ext2_inode_table inode_t = read_inode(inode_num);
    uint32_t* blocks = (uint32_t*)inode_t.i_block;
    uint32_t data_block = blocks[0];

    uint8_t buf[1024];
    for (int i = 0; i < sectors_per_block; i++) {
        ata_read28(data_block * sectors_per_block + i, buf + (i * 512), EXT2_DISK_DRIVE);
    }

    int pos = 0;
    while (pos < block_size)
    {
        struct ext2_dir_entry* entry = (struct ext2_dir_entry*)(buf + pos);

        if (entry->inode != 0)
        {

            char name[256];
            kmemcpy(entry->name, name, entry->name_len);    // Copy name bytes
            name[entry->name_len] = '\0';                                   // Add null terminator

            serial_print(name);
            serial_print("\r\n");
        }
        pos += entry->rec_len;
    }
}

void ext2_read_file(uint32_t inode_num, char* dest)
{
    struct ext2_inode_table inode = read_inode(inode_num);
    uint32_t bytes_left = inode.i_size;
    char* ptr = dest;
    uint32_t block_idx = 0;

    // Direct blocks (0-11)
    while (bytes_left > 0 && block_idx < 12)
    {
        uint8_t buf[1024];
        uint32_t sector = inode.i_block[block_idx] * sectors_per_block;
        for (uint32_t s = 0; s < sectors_per_block; s++)
            ata_read28(sector + s, buf + s * 512, EXT2_DISK_DRIVE);

        uint32_t to_copy = bytes_left < block_size ? bytes_left : block_size;
        kmemcpy(ptr, buf, to_copy);
        ptr += to_copy;
        bytes_left -= to_copy;
        block_idx++;
    }

    // Singly indirect block (i_block[12])
    if (bytes_left > 0 && inode.i_block[12] != 0)
    {
        uint8_t indirect_buf[1024];
        uint32_t sector = inode.i_block[12] * sectors_per_block;
        for (uint32_t s = 0; s < sectors_per_block; s++)
            ata_read28(sector + s, indirect_buf + s * 512, EXT2_DISK_DRIVE);

        uint32_t* indirect_blocks = (uint32_t*)indirect_buf;
        uint32_t ptrs_per_block = block_size / 4;

        for (uint32_t i = 0; i < ptrs_per_block && bytes_left > 0; i++)
        {
            if (indirect_blocks[i] == 0) break;

            uint8_t buf[1024];
            uint32_t bsector = indirect_blocks[i] * sectors_per_block;
            for (uint32_t s = 0; s < sectors_per_block; s++)
                ata_read28(bsector + s, buf + s * 512, EXT2_DISK_DRIVE);

            uint32_t to_copy = bytes_left < block_size ? bytes_left : block_size;
            kmemcpy(ptr, buf, to_copy);
            ptr += to_copy;
            bytes_left -= to_copy;
        }
    }

    // Doubly indirect block (i_block[13])
    if (bytes_left > 0 && inode.i_block[13] != 0)
    {
        uint8_t dbl_buf[1024];
        uint32_t sector = inode.i_block[13] * sectors_per_block;
        for (uint32_t s = 0; s < sectors_per_block; s++)
            ata_read28(sector + s, dbl_buf + s * 512, EXT2_DISK_DRIVE);

        uint32_t* dbl_blocks = (uint32_t*)dbl_buf;
        uint32_t ptrs_per_block = block_size / 4;

        for (uint32_t i = 0; i < ptrs_per_block && bytes_left > 0; i++)
        {
            if (dbl_blocks[i] == 0) break;

            uint8_t indirect_buf[1024];
            uint32_t isector = dbl_blocks[i] * sectors_per_block;
            for (uint32_t s = 0; s < sectors_per_block; s++)
                ata_read28(isector + s, indirect_buf + s * 512, EXT2_DISK_DRIVE);

            uint32_t* indirect_blocks = (uint32_t*)indirect_buf;

            for (uint32_t j = 0; j < ptrs_per_block && bytes_left > 0; j++)
            {
                if (indirect_blocks[j] == 0) break;

                uint8_t buf[1024];
                uint32_t bsector = indirect_blocks[j] * sectors_per_block;
                for (uint32_t s = 0; s < sectors_per_block; s++)
                    ata_read28(bsector + s, buf + s * 512, EXT2_DISK_DRIVE);

                uint32_t to_copy = bytes_left < block_size ? bytes_left : block_size;
                kmemcpy(ptr, buf, to_copy);
                ptr += to_copy;
                bytes_left -= to_copy;
            }
        }
    }

    *ptr = '\0';
}
