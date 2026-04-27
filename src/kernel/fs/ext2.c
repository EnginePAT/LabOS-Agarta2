#include <kernel/fs/ext2.h>
#include <kernel/disk/ata.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/vga/serial.h>
#include <util/mem.h>
#include <stdint.h>

#define EXT2_DISK_DRIVE 1                   // Master 8, drive 1 (0x81)

static struct ext2_super_block sb;
static struct ext2_block_group_desc bgd;
static uint32_t block_size;
static uint32_t sectors_per_block;

void ext2_init()
{
    // Read superblock
    // NOTE: The superblock is always at byte 1024 (sector 2)
    uint8_t sb_buf[1024];
    ata_read28(2, sb_buf, EXT2_DISK_DRIVE);

    // Copy superblock (can't use pointer directly, stack gets clobbered)
    memcpy(sb_buf, (uint8_t*)&sb, (int)sizeof(struct ext2_super_block));

    // Verify the superblock to ensure the filesystem is indeed EXT2
    if (sb.s_magic != EXT2_MAGIC)
    {
        vga_print("ext2: invalid magic!\n");
        return;
    }

    block_size = EXT2_BLOCK_SIZE(&sb);
    sectors_per_block = block_size / 512;           // 512 bytes per sector

    serial_print("ext2: block size = ");
    serial_print_hex(block_size);
    serial_print("\r\n");

    // Read block group descriptor table
    // Always in block 2 for 1KB blocks, block 1 otherwise
    uint32_t bgdt_block = (block_size == 1024) ? 2 : 1;

    uint8_t bgdt_buf[1024];
    ata_read28(bgdt_block * sectors_per_block, bgdt_buf, EXT2_DISK_DRIVE);
    memcpy(bgdt_buf, (uint8_t*)&bgd, (int)sizeof(struct ext2_block_group_desc));

    serial_print("ext2: inodate table @ block ");
    serial_print_hex(bgd.bg_inode_table);
    serial_print("\r\n");
    serial_print("ext2: inodes per group = ");
    serial_print_hex(sb.s_inodes_per_group);
    serial_print("\r\n");

    vga_print("EXT2 mounted!\n");

    // Read the root inode and get its size
    read_inode(2);
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
    ata_read28(sector, buf, EXT2_DISK_DRIVE);
    memcpy(buf + byte_offset, (uint8_t*)&inode_t, (int)sizeof(struct ext2_inode_table));

    serial_print("ext2: inode ");
    serial_print_hex(inode_num);
    serial_print(" is ");
    serial_print_hex(inode_t.i_size);
    serial_print(" bytes!\n");

    return inode_t;
}
