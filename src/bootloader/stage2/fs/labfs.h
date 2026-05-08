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
/*
    LabFS-Lite
    Copyright 2026 EnginePAT. All rights resvered.
    Licensed under the Apache 2.0 license. See LICENSE for more details.

    LabFS-Lite is a filesystem specifically designed for BIOS bootloaders.
    Currently supports 1 directory, and up to 3 files.
*/
#ifndef LABFS_H
#define LABFS_H

#include <stdint.h>

#define LABFS_MAGIC 0x4fB6
#define LABFS_VERSION 1
#define LABFS_MAX_FILENAME 56
#define LABFS_BLOCK_SIZE 512
#define LABFS_TYPE_FILE 1
#define LABFS_TYPE_DIR  2

#define LABFS_MAX_FILES 16      // max inodes (bump from 3)
#define LABFS_MAX_DIRENTS 16    // max dir entries

#define LABFS_DIRENTRIES_PER_BLOCK (LABFS_BLOCK_SIZE / sizeof(struct DirEntry))

struct Superblock {
    uint32_t magic;             // Magic = 0x4FB6
    uint8_t version;            // Version = 1

    uint8_t resv;               // 1 byte padding

    uint32_t block_size;
    uint32_t inode_start;       // Where the inode table starts
    uint32_t data_start;        // Where the data starts
    uint32_t inode_count;       // How many inodes?
    uint32_t root_inode;        // Where is the root inode?

    uint8_t reserved[2];        // 2 bytes padding
} __attribute__((packed));

struct Inode {
    uint32_t type;
    uint32_t size;
    uint32_t start_block;
    uint32_t block_count;
} __attribute__((packed));

struct DirEntry {
    char name[LABFS_MAX_FILENAME];
    uint32_t inode_idx;
} __attribute__((packed));

struct dir_entry_t {
    const char* name;
    struct Inode* inode;
} __attribute__((packed));

#endif      // LABFS_H
