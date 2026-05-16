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
#ifndef EXT2_VFS_H
#define EXT2_VFS_H

#include <stdint.h>

#define S_IFMT   0xF000
#define S_IFDIR  0x4000
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

struct vfs_node* ext2_mount_root();
static struct vfs_node* ext2_vfs_finddir(struct vfs_node* node, const char* name);
static uint32_t ext2_vfs_read(struct vfs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer);

#endif      // EXT2_VFS_H
