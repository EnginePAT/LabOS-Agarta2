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
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(void);

void serial_out(char c);
void serial_print(const char* s);
void serial_hex_dump(uint8_t* buf, int len);
void serial_print_hex(uint32_t val);

#endif      // SERIAL_H
