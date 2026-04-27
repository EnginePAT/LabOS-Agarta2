#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

void mouse_init();
void mouse_draw(int x, int y);

static uint8_t mouse_read();
static void mouse_write(uint8_t value);
static void mouse_wait(uint8_t type);

#endif      // MOUSE_H
