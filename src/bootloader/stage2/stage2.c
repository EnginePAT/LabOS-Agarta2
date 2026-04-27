#include "fs/labfs.h"
#include <stdint.h>
#include <util/util.h>
#include "disk/ata.h"
#include "serial.h"
#include <kernel/boot_info.h>

volatile uint8_t* video = (uint8_t*)0xb8000;

struct BootConfig {
    int version;
    char path[64];
};

void print(const char* s)
{
    for (int i = 0; s[i] != '\0'; i++)
    {
        // Print a character to the screen located at s[i]
        video[i * 2] = s[i];
        video[i * 2 + 1] = 0x0f;
    }
}

int strcmp(const char* a, const char* b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

char* substring(char* buffer, int start, int end)
{
    static char result[128];
    int pos = 0;

    for (int i = start; i <= end; i++)
    {
        result[pos++] = buffer[i];
    }
    result[pos] = '\0';
    return result;
}

int atoi(char* str)
{
    int res = 0;
    for (int i = 0; str[i] != '\n' && str[i] != '\r' && str[i] != '\0'; ++i)
    {
        res = res * 10 + str[i] - '0';
    }
    return res;
}

void get_kernel_path(char* buffer, struct BootConfig* bcfg)
{
    int pos = 0;
    while (buffer[pos] != 0)
    {
        if (buffer[pos] == '\n')
        {
            pos++;
            continue;
        }

        if (buffer[pos] == ':') pos++;

        // key
        char key[32];
        int k = 0;
        int key_start = pos;
        while (buffer[pos] != '=' && buffer[pos] != '\n' && buffer[pos] != 0 && k < (int)sizeof(key) - 1) {
            key[k++] = buffer[pos++];
        }
        key[k] = '\0';

        if (buffer[pos] != '=') {
            while (buffer[pos] != '\n' && buffer[pos] != 0) pos++;
            continue;
        }
        pos++; // skip '='

        // value
        char val[128];
        int v = 0;
        while (buffer[pos] != '\n' && buffer[pos] != 0 && v < (int)sizeof(val) - 1) {
            val[v++] = buffer[pos++];
        }
        val[v] = '\0';

        if (strcmp(key, "version") == 0)
        {
            bcfg->version = atoi(val);
        } else if (strcmp(key, "kernel") == 0)
        {
            int i = 0;
            for (i = 0; val[i] != '\0'; i++)
            {
                bcfg->path[i] = val[i];
            }
            bcfg->path[i] = '\0';
        }
    }
}

int resolve_path(char* path, struct Inode* inodes, struct Superblock* sb)
{
    // Strip leading slash
    int pos = 0;
    if (path[pos] == '/') pos++;

    int current_inode = sb->root_inode;

    while (path[pos] != '\0')
    {
        // Extract next path component
        char name[64];
        int n = 0;
        while (path[pos] != '/' && path[pos] != '\0' && n < 63)
            name[n++] = path[pos++];
        name[n] = '\0';
        if (path[pos] == '/') pos++;

        // Search current dir's data block for this name
        struct Inode* dir = &inodes[current_inode];
        uint8_t dirbuf[512];
        ata_read28(dir->start_block, dirbuf, 0);
        struct DirEntry* entries = (struct DirEntry*)dirbuf;

        int found = -1;
        for (int i = 0; i < LABFS_DIRENTRIES_PER_BLOCK; i++)
        {
            if (entries[i].inode_idx == 0) continue;
            if (strcmp(entries[i].name, name) == 0)
            {
                found = entries[i].inode_idx;
                break;
            }
        }

        if (found < 0) {
            serial_print("resolve_path: not found: ");
            serial_print(name);
            serial_print("\r\n");
            return -1;
        }
        current_inode = found;
    }
    return current_inode;
}

void* memcpy(void* dest, const void* src, int n)
{
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (int i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}

extern void stage2_main(unsigned int magic, unsigned int addr, unsigned int fb, unsigned int width, unsigned int height, unsigned int pitch, unsigned int bpp)
{
    serial_init();
    serial_print("Stage2 started\r\n");
    print("Stage 2 bootloader started!");

    uint8_t buffer[512];
    ata_read28(0, buffer, 0);
    struct Superblock sb;
    memcpy(&sb, buffer + 3, sizeof(struct Superblock));

    // Verify the filesystem is indeed LabFS-Lite
    if (sb.magic != LABFS_MAGIC || sb.version > LABFS_VERSION)
    {
        serial_print("Error: Not a valid filesystem.\n");
        while (1);
    }

    // Now we can look for /config.cfg and parse it
    String config = "config.cfg";
    uint8_t inode_buf[512];                             // Enough for small inode tables for now
    ata_read28(sb.inode_start, inode_buf, 0);

    struct Inode* inodes = (struct Inode*)inode_buf;
    struct Inode* root = &inodes[sb.root_inode];       // Or 0 if root is fixed

    uint8_t dirent_buf[512];
    ata_read28(sb.data_start, dirent_buf, 0);
    struct DirEntry* entries = (struct DirEntry*)dirent_buf;

    struct DirEntry* cfg = 0;
    for (int i = 0; i < LABFS_DIRENTRIES_PER_BLOCK * 2; i++)
    {
        if (entries[i].inode_idx == 0)
            continue;

        if (strcmp(entries[i].name, config) == 0)
        {
            cfg = &entries[i];
        }
    }

    if (!cfg) {
        serial_print("config.cfg not found\r\n");
        while (1);
    }

    struct Inode* cfg_inode = &inodes[cfg->inode_idx];

    uint8_t cfg_buf[LABFS_BLOCK_SIZE];
    ata_read28(cfg_inode->start_block, cfg_buf, 0);

    // Now we can parse the config file
    struct BootConfig boot_config;
    get_kernel_path((char*)cfg_buf, &boot_config);

    // Now we can loop over the filesystem, and find kernel.bin
    char* kernel_path = boot_config.path;
    int kernel_inode_idx = resolve_path(kernel_path, inodes, &sb);
    if (kernel_inode_idx < 0)
    {
        serial_print("kernel not found!\r\n");
        while (1);
    }

    // Load the kernel into memory at 0x100000 (1MB)
    struct Inode* kernel_inode = &inodes[kernel_inode_idx];
    uint8_t* kernel_addr = (uint8_t*)0x100000;

    for (uint32_t i = 0; i < kernel_inode->block_count; i++)
    {
        ata_read28(kernel_inode->start_block + i, kernel_addr + (i * 512), 0);
    }
    serial_print("Kernel loaded!\n");

    // Jump to kernel (passing LBootInfo eventually)
    struct LBootInfo* boot_info = (struct LBootInfo*)0x500;
    struct LFramebufferInfo* fb_info = (struct LFramebufferInfo*)0x600;
    boot_info->magic       = magic;
    boot_info->addr         = 

    fb_info->framebuffer = fb;
    fb_info->width       = width;
    fb_info->height      = height;
    fb_info->pitch       = pitch;
    fb_info->bpp         = bpp;

    typedef void (*KernelEntry)(struct LBootInfo*, struct LFramebufferInfo*);
    KernelEntry kernel_entry = (KernelEntry)kernel_addr;
    kernel_entry(boot_info, fb_info);

    while (1);
}
