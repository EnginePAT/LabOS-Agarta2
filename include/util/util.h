#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define CEIL_DIV(a,b) (((a + b) - 1)/b)

typedef const char* String;     // String type so we don't have to type 'const char*' all the time
typedef const char cchar;       // Const string type

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

struct InterruptRegisters{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};


#endif      // UTIL_H
