#include <kernel/fs/vfs.h>
#include <util/string.h>
#include <kernel/core/vga/serial.h>

vfs_node_t* vfs_root = 0;

static const char* skip_slash(const char* p) {
    while (*p == '/') p++;
    return p;
}

vfs_node_t* vfs_open(const char* path) {
    if (!vfs_root || !path || path[0] != '/')
        return 0;

    vfs_node_t* node = vfs_root;
    const char* p = skip_slash(path);

    char name[64];

    while (*p) {
        int i = 0;
        while (*p && *p != '/' && i < 63) {
            name[i++] = *p++;
        }
        name[i] = '\0';
        p = skip_slash(p);

        serial_print("[vfs_open] component: ");
        serial_print(name);
        serial_print("\n");

        if (!node->finddir)
            return 0;

        node = node->finddir(node, name);
        if (!node)
            return 0;
    }

    return node;
}

uint32_t vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !node->read)
        return 0;
    return node->read(node, offset, size, buffer);
}
