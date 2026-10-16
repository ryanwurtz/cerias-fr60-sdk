#!/bin/bash

set -euo pipefail

proj_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
tools_dir="$proj_dir/.tools"
prefix="$tools_dir/cross"
gcc_version=14.2.0
binutils_version=2.43
jobs=${JOBS:-$(nproc)}

# dependecies
sudo apt-get update
sudo apt-get install -y build-essential cmake curl ca-certificates xz-utils \
    flex bison texinfo libgmp-dev libmpfr-dev libmpc-dev zlib1g-dev

mkdir -p "$tools_dir/downloads" "$tools_dir/build" "$prefix"
export PATH="$prefix/bin:$PATH"

# source code download
for package in "binutils-$binutils_version" "gcc-$gcc_version"; do
    if [[ ! -d "$tools_dir/build/$package" ]]; then
        source_dir="${package%%-*}"
        if [[ "$source_dir" == gcc ]]; then
            source_dir="$source_dir/$package"
        fi
        curl -fL --retry 3 "https://ftp.gnu.org/gnu/$source_dir/$package.tar.xz" \
            -o "$tools_dir/downloads/$package.tar.xz"
        tar -xJf "$tools_dir/downloads/$package.tar.xz" -C "$tools_dir/build"
    fi
done

# assembler and linker
mkdir -p "$tools_dir/build/binutils-build"
(
    cd "$tools_dir/build/binutils-build"
    if [[ ! -f Makefile ]]; then
        "../binutils-$binutils_version/configure" --target=fr30-elf \
            --prefix="$prefix" --disable-nls --disable-werror
    fi
    make -j "$jobs"
    make install
)

# build cross compiler
mkdir -p "$tools_dir/build/gcc-build"
(
    cd "$tools_dir/build/gcc-build"
    if [[ ! -f Makefile ]]; then
        "../gcc-$gcc_version/configure" --target=fr30-elf --prefix="$prefix" \
            --enable-languages=c --without-headers --disable-nls --disable-multilib \
            --disable-shared --disable-threads --disable-libssp \
            --disable-libquadmath --disable-libatomic
    fi
    make -j "$jobs" all-gcc
    make install-gcc
    make -j "$jobs" all-target-libgcc
    make install-target-libgcc
)

printf '\nCross compiler installed in %s/bin\n' "$prefix"
printf 'To use it in your current shell, run:\n  export PATH=%q:"$PATH"\n' "$prefix/bin"
