#ifndef EXIT_HANDLER_H
#define EXIT_HANDLER_H

#include <stdint.h>

int sys_exit_handler(uint32_t code, uint32_t, uint32_t, uint32_t, uint32_t);
int sys_write_handler(uint32_t fd, uint32_t buf, uint32_t len, uint32_t, uint32_t);

#endif      // EXIT_HANDLER_H
