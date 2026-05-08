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
#include <userspace/core/mach4/loader.h>
#include <stdint.h>

static uint32_t user_read_write     = 0xFFFFFFFF;
static uint32_t user_read_only      = 0xFFFF0000;
static uint32_t admin_write_only    = 0xFFFFAAAA;
static uint32_t admin_only          = 0x0000AAAA;


