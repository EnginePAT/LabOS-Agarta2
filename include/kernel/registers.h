/*
    registers.h => The 32-bit registers LabOS will use
*/
#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef struct registers_t {    
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_ignored;

    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} registers_t;


#endif      // REGISTERS_H
