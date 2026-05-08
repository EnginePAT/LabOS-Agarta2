#include <kernel/fs/ext2.h>
#include <kernel/fs/vfs.h>
#include <util/string.h>
#include <util/mem.h>
#include <stdint.h>
#include <kernel/core/vga/serial.h>
#include <kernel/fs/ext2_vfs.h>

extern struct ext2_super_block sb;

static uint32_t ext2_vfs_read(struct vfs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer);
static struct vfs_node* ext2_vfs_finddir(struct vfs_node* node, const char* name);

struct vfs_node* ext2_mount_root() {
    static struct vfs_node root;

    memset(&root, 0, sizeof(root));
    root.flags   = VFS_FLAG_DIR;
    root.inode   = 2;          // EXT2 root inode
    root.fs_data = &sb;
    root.read    = 0;          // dirs don't support read()
    root.finddir = ext2_vfs_finddir;

    strcpy(root.name, "/");

    return &root;
}

static struct vfs_node* ext2_vfs_finddir(struct vfs_node* node, const char* name) {
    uint32_t child_inode_num = ext2_find_entry(node->inode, name);
    if (child_inode_num == 0)
        return 0;

    static struct vfs_node child;
    memset(&child, 0, sizeof(child));

    child.inode   = child_inode_num;
    child.fs_data = node->fs_data;
    struct ext2_inode_table ino = read_inode(child_inode_num);   // you likely already have this
    child.size = ino.i_size;

    if (S_ISDIR(ino.i_mode)) {                // or (ino.i_mode & 0x4000)
        child.flags   = VFS_FLAG_DIR;
        child.finddir = ext2_vfs_finddir;
        child.read    = 0;
    } else {
        child.flags = VFS_FLAG_FILE;
        child.read  = ext2_vfs_read;
        child.finddir = 0;
    }

    strncpy(child.name, name, 63);
    child.name[63] = '\0';

    return &child;
}

static uint32_t ext2_vfs_read(struct vfs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    static uint8_t tmp[4096];
    ext2_read_file(node->inode, (char*)tmp);
    
    uint32_t len = node->size;  // use actual inode size, not strlen
    if (offset >= len) return 0;
    
    uint32_t to_copy = len - offset;
    if (to_copy > size) to_copy = size;  // respect caller's size limit
    
    kmemcpy(buffer, tmp + offset, to_copy);
    return to_copy;
}
