#include <userspace/core/mach4/loader.h>
#include <stdint.h>

static uint32_t user_read_write     = 0xFFFFFFFF;
static uint32_t user_read_only      = 0xFFFF0000;
static uint32_t admin_write_only    = 0xFFFFAAAA;
static uint32_t admin_only          = 0x0000AAAA;


