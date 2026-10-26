#!/bin/bash

set -e

#global args
program=multest
test=false
build=false
clean=false
analyze_elf=false
install_ghidra_requested=false
install_gcc_requested=false
install_newlib_requested=false
proj_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
ghidra_dir=${GHIDRA_INSTALL_DIR:-$proj_dir/.tools/ghidra}
cross_prefix=${CROSS_PREFIX:-$proj_dir/.tools/cross/bin/fr30-elf-}

parse_args() {
    while (($#)); do
        case "$1" in
            -p)
                if (($# < 2)) || [[ -z "$2" || "$2" == --* ]]; then
                    printf 'Missing value for %s\n' "$1" >&2
                    exit 2
                fi
                test=true
                program=$2
                shift 2
                ;;
            -i)
                shift
                if (($# == 0)) || [[ "$1" == -* ]]; then
                    printf 'Missing installation name for -i (ghidra, gcc, newlib)\n' >&2
                    exit 2
                fi
                while (($#)) && [[ "$1" != -* ]]; do
                    case "$1" in
                        ghidra) install_ghidra_requested=true ;;
                        gcc) install_gcc_requested=true ;;
                        newlib) install_newlib_requested=true ;;
                        *)
                            printf 'Unknown installation: %s (expected ghidra, gcc, newlib)\n' "$1" >&2
                            exit 2
                            ;;
                    esac
                    shift
                done
                ;;
            -b)
                build=true
                shift 1
                ;;
            -c)
                clean=true
                shift 1
                ;;
            --elf)
                analyze_elf=true
                shift
                ;;
            -h)
                echo "Usage: $0 [-i <ghidra|gcc|newlib> ...] [-p <binary>] [--elf] [-b] [-c]"
                echo "  -i  Install one or more tools (may be repeated)"
                echo "  -p  Compile and analyze a test program"
                echo "  --elf  Analyze the ELF with symbols (DWARF disabled); write results to <program>/elf/"
                echo "  -b  Rebuild and install the Ghidra extension"
                echo "  -c  Clean generated builds and test output"
                exit 0
                ;;
            *)
                printf 'Unknown argument: %s\n' "$1" >&2
                exit 2
                ;;
        esac
    done
}

install_ghidra() (
    set -euo pipefail
    tools_dir="$proj_dir/.tools"
    version=12.1.4
    archive="ghidra_${version}_PUBLIC_20260921.zip"

    # Install dependencies.
    sudo apt-get update
    sudo apt-get install -y curl ca-certificates unzip openjdk-21-jdk

    mkdir -p "$tools_dir/downloads"

    # Download and extract the official release.
    if [[ ! -d "$tools_dir/ghidra_${version}_PUBLIC" ]]; then
        curl -fL --retry 3 \
            "https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_${version}_build/$archive" \
            -o "$tools_dir/downloads/$archive"
        unzip -q "$tools_dir/downloads/$archive" -d "$tools_dir"
    fi

    # Give run.sh a stable installation path.
    ln -sfn "ghidra_${version}_PUBLIC" "$tools_dir/ghidra"
)

install_gcc() (
    set -euo pipefail
    tools_dir="$proj_dir/.tools"
    prefix="$tools_dir/cross"
    gcc_version=14.2.0
    binutils_version=2.43
    jobs=${JOBS:-$(nproc)}

    # dependecies
    sudo apt-get update
    sudo apt-get install -y build-essential curl ca-certificates xz-utils \
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
)

install_newlib() (
    set -euo pipefail
    newlib_version=4.6.0.20260123
    mkdir -p "$proj_dir/.tools/downloads" "$proj_dir/.tools/build"
    cd -- "$proj_dir"
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
)

run() {
    local program_elf_path="$proj_dir/tests/output/$program/${program}.elf"
    local program_bin_path="$proj_dir/tests/output/$program/${program}.bin"
    local analyze="$ghidra_dir/support/analyzeHeadless"
    local output_dir="$proj_dir/tests/output/$program/bin"
    local -a import_args=(-import "$program_bin_path" -overwrite
        -loader BinaryLoader -loader-baseAddr 0x40000)
    local -a analysis_args=(-preScript DisableDwarf.java)
    if $analyze_elf; then
        output_dir="$proj_dir/tests/output/$program/elf"
        import_args=(-import "$program_elf_path" -overwrite -loader ElfLoader)
    fi

    #compile
    mkdir -p "$proj_dir/ghidra_proj" "$output_dir"
    cmake -S "$proj_dir/sdk" -B "$proj_dir/build/tests" \
        -DCMAKE_TOOLCHAIN_FILE="$proj_dir/sdk/fr60-toolchain.cmake" \
        -DCROSS_PREFIX="$cross_prefix" \
        -DFR60_TEST_PROGRAM="$program"
    cmake --build "$proj_dir/build/tests" --target "$program"
    "${cross_prefix}objdump" -d "$program_elf_path" > "$proj_dir/tests/output/$program/objdump.S"
    "${cross_prefix}objcopy" -O binary "$program_elf_path" "$program_bin_path"

    #decompile
    "$analyze" "$proj_dir/ghidra_proj" fr60-test \
        "${import_args[@]}" \
        -processor FR:BE:32:60 \
        -cspec gcc \
        -scriptPath "$proj_dir/fr60/ghidra_scripts" \
        "${analysis_args[@]}" \
        -preScript ImportMmio.java \
        "$proj_dir/fr60/ghidra_scripts/boards/mb91f467ba.xml" \
        -preScript SeedBootEntry.java 0xF4000 \
        -postScript ExportDecompiled.java \
        "$output_dir/decomp.c" \
        "$output_dir/disassembly.S" \
        "$output_dir/p-code.txt"
}

build() {
    local extension_dir="$proj_dir/fr60"
    local ghidra_version ghidra_release install_dir
    ghidra_version=$(awk -F= '$1 == "application.version" {print $2}' "$ghidra_dir/Ghidra/application.properties")
    ghidra_release=$(awk -F= '$1 == "application.release.name" {print $2}' "$ghidra_dir/Ghidra/application.properties")
    install_dir="${XDG_CONFIG_HOME:-$HOME/.config}/ghidra/ghidra_${ghidra_version}_${ghidra_release}/Extensions"
    local archive

    (
        cd -- "$extension_dir/data"
        "$ghidra_dir/support/sleigh" -i sleighArgs.txt -a languages
    )

    "$ghidra_dir/support/gradle/gradlew" -p "$extension_dir" \
        -PGHIDRA_INSTALL_DIR="$ghidra_dir" clean buildExtension --rerun-tasks

    #install most recent build
    archive=$(ls -t "$extension_dir"/dist/*.zip | head -n 1)
    mkdir -p -- "$install_dir"
    unzip -o "$archive" -d "$install_dir"
}

clean() {
    rm -rf "$proj_dir/ghidra_proj" "$proj_dir/build"
    rm -rf "$proj_dir/fr60/.gradle" "$proj_dir/fr60/build" "$proj_dir/fr60/dist"
    rm -rf "$proj_dir/tests/output/"
}

parse_args "$@"
if $clean; then
    clean
fi
if $install_ghidra_requested; then
    install_ghidra
fi
if $install_gcc_requested; then
    install_gcc
fi
if $install_newlib_requested; then
    install_newlib
fi
if $build; then
    build
fi
if $test; then
    run
fi
