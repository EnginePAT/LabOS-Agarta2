// util/string.h
#ifndef STRING_H
#define STRING_H

#include <stdint.h>

static inline void strcpy(char* dst, const char* src) {
    while ((*dst++ = *src++));
}

static inline void strncpy(char* dst, const char* src, uint32_t n) {
    while (n-- && (*dst++ = *src++));
}

static inline int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

#endif
