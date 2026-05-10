#
# * LabOS Agarta
# * Copyright (c) 2026 EnginePAT
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *     http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
#!/bin/bash

set -e

echo "Installing base components"
sudo apt update
sudo apt install -y nasm qemu-system build-essential \
    bison flex libgmp3-dev libmpfr-dev libmpc-dev texinfo wget

echo "Setting up directories"
sudo mkdir -p /opt/cross/i386
sudo chown -R $USER:$USER /opt/cross

export PREFIX=/opt/cross/i386
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

cd /opt/cross/i386

echo "Downloading sources"
wget -nc https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.xz
wget -nc https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.xz

tar -xf binutils-2.42.tar.xz
tar -xf gcc-14.2.0.tar.xz

echo "Building Binutils"
mkdir -p build-binutils && cd build-binutils
../binutils-2.42/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --disable-werror

make -j$(nproc)
make install

cd ..

echo "Building GCC"
mkdir -p build-gcc && cd build-gcc
../gcc-14.2.0/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --enable-languages=c,c++ \
    --without-headers

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)

make install-gcc
make install-target-libgcc

echo "Verifying"
which i386-elf-gcc
i386-elf-gcc --version
