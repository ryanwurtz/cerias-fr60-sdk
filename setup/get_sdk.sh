#!/bin/bash

set -euo pipefail

proj_dir="$PWD"
newlib_version=4.6.0.20260123
export PATH="$proj_dir/.tools/cross/bin:$PATH"

#source download and unzip
curl -fL --retry 3 \
    "https://sourceware.org/pub/newlib/newlib-$newlib_version.tar.gz" \
    -o ".tools/downloads/newlib-$newlib_version.tar.gz"
tar -xzf ".tools/downloads/newlib-$newlib_version.tar.gz" \
    -C .tools/build

#configure for fr30 with manually provided stubs
mkdir -p .tools/build/newlib-build
cd .tools/build/newlib-build
"../newlib-$newlib_version/configure" \
    --target=fr30-elf \
    --prefix="$proj_dir/.tools/cross" \
    --disable-multilib \
    --disable-libgloss \
    --disable-newlib-supplied-syscalls

#build
sudo apt-get update
sudo apt-get install -y cmake
make all-target-newlib
make install-target-newlib