#include <stdint.h>
#include <util/mem.h>

void memset(void *dest, char val, uint32_t count)
{
    char* temp = (char*)dest;
    for (; count != 0; count--)
    {
        *temp++ = val;
    }
}

void memcpy(void* src, void* dest, uint32_t count)
{
    uint8_t* d = (uint8_t*)dest;
    uint8_t* s = (uint8_t*)src;
    for (int i = 0; i < count; i++)
    {
        d[i] = s[i];
    }
}
