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
#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define VFS_FLAG_FILE   0x1
#define VFS_FLAG_DIR    0x2

struct vfs_node;

typedef uint32_t (*vfs_read_t)(struct vfs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer);
typedef struct vfs_node* (*vfs_finddir_t)(struct vfs_node* node, const char* name);

typedef struct vfs_node {
    char name[64];
    uint32_t flags;      // VFS_FLAG_FILE / VFS_FLAG_DIR
    uint32_t size;       // optional for now
    uint32_t inode;      // FS-specific inode number

    void* fs_data;       // FS-specific context (e.g. ext2 superblock)

    vfs_read_t    read;
    vfs_finddir_t finddir;
} vfs_node_t;

extern vfs_node_t* vfs_root;

vfs_node_t* vfs_open(const char* path);
uint32_t vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

#endif
