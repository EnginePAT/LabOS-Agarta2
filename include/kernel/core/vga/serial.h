#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(void);

void serial_out(char c);
void serial_print(const char* s);
void serial_hex_dump(uint8_t* buf, int len);
void serial_print_hex(uint32_t val);

#endif      // SERIAL_H
