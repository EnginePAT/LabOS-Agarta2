#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>

#define EXT2_MAGIC 0xEF53
#define EXT2_BLOCK_SIZE(sb)     (1024 << (sb)->s_log_block_size)
#define EXT2_INODE_SIZE(sb)     ((sb)->s_inode_size)

#define EXT2_S_IFREG            0x8000      // Regular file
#define EXT2_S_IFDIR            0x4000      // Directory

struct ext2_super_block {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;

    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;                           // Magic identifier
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;

    // -- EXT2_DYNAMIC_REV Specific --
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t s_uuid[16];                 // 16-byte UUID
    uint8_t s_volume_name[16];          // 16-byte Volume Name
    char s_last_mounted[64];
    uint32_t s_algo_bitmap;

    // -- Performance Hints --
    uint8_t s_prealloc_blocks;
    uint8_t s_prealloc_dir_blocks;
    uint16_t alignment;

    // -- Journalling Support --
    uint8_t s_journal_uuid[16];
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;

    // -- Directory Indexing Support --
    uint32_t s_hash_seed[4];
    uint8_t s_def_hash_version;
    uint8_t padding[3];

    // -- Other Options --
    uint32_t s_default_mount_options;
    uint32_t s_first_meta_bg;
    uint8_t reserved[760];
} __attribute__((packed));

struct ext2_block_group_desc {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint8_t bg_reserved[12];
} __attribute__((packed));

struct ext2_inode_table {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;       // Accessed Time
    uint32_t i_ctime;       // Created Time
    uint32_t i_mtime;       // Modified Time
    uint32_t i_dtime;       // Deleted Time
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint8_t i_osd2[12];
} __attribute__((packed));

struct ext2_dir_entry {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    char name[0];
} __attribute__((packed));

void ext2_init();
struct ext2_inode_table read_inode(uint32_t inode_num);
void read_entries(uint32_t inode_num);
void ext2_read_file(uint32_t inode_num, char dest[]);
uint32_t ext2_find_entry(uint32_t dir_inode, const char* name);

#endif      // EXT2_H
