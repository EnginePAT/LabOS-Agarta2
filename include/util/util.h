#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

typedef const char* String;     // String type so we don't have to type 'const char*' all the time
typedef const char cchar;       // Const string type

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

#endif      // UTIL_H
