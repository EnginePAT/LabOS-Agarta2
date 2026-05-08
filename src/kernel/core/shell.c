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
#include <kernel/core/shell.h>
#include <kernel/core/keyboard.h>
#include <kernel/core/vga/vga.h>
#include <util/util.h>

void shell_main()
{
    vga_print("Welcome to LabOS Agarta!\n\n");
    vga_print("[Agarta]: ~$ ");

    while (1)
    {
        asm volatile ("hlt");       // Wait for IRQ1 to fire

        if (command_ready)
        {
            char* cmd = keyboard_get_command();
            if (strcmp(cmd, "Hello") == 0)
            {
                vga_print("Hello, world!\n");
            }

            // More commands as necessary

            vga_print("[Agarta]: ~$ ");
            command_ready = 0;      // Clear the flag
        }
    }
}
