#ifndef USERSPACE_H
#define USERSPACE_H

#include <stdint.h>

void userspace_init();
extern void jump_usermode(uint32_t entry, uint32_t stack);

#endif      // USERSPACE_H
