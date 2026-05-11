/*
 * LabOS Agarta
 * Copyright (c) 2026 EnginePAT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <kernel/core/mm/pit.h>
#include <kernel/core/mm/idt.h>
#include <util/mem.h>
#include <util/util.h>
#include <kernel/core/multitasking/scheduler.h>

uint64_t ticks;
const uint32_t freq = 1000;

void onIrq0(struct InterruptRegisters *regs)
{
    ticks += 1;
    outb(0x20, 0x20);
    schedule(regs);
}

void initTimer()
{
    ticks = 0;
    irq_install_handler(0,&onIrq0);

    // 119318.16666 Mhz
    uint32_t divisor = 1193180/freq;

    // 0011 0110
    outb(0x43,0x36);
    outb(0x40,(uint8_t)(divisor & 0xFF));
    outb(0x40,(uint8_t)((divisor >> 8) & 0xFF));
}
