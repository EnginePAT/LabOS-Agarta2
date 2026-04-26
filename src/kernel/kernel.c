#include <stdint.h>

volatile uint8_t* video = (uint8_t*)0xb8000;

void kernel_main(unsigned int magic, uint8_t* addr)
{
    // Do nothing for now
    video[0] = 'A';
    video[1] = 0x0f;
    while (1);
}
