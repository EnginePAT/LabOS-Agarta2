#ifndef PIC_H
#define PIC_H

#include <util/util.h>

void initTimer();
void onIrq0(struct InterruptRegisters *regs);

#endif      // PIC_H
