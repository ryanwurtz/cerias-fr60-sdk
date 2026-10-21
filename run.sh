#!/bin/bash

set -e

#global args
program=multest
test=false
build=false
clean=false
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
            -b)
                build=true
                shift 1
                ;;
            -c)
                clean=true
                shift 1
                ;;
            -h)
                echo "Usage: $0 [-p <binary>] [-b (rebuild and install extension)]"
                exit 0
                ;;
            *)
                printf 'Unknown argument: %s\n' "$1" >&2
                exit 2
                ;;
        esac
    done
}

run() {
    local program_bin_path="$proj_dir/tests/output/$program/${program}.elf"
    local analyze="$ghidra_dir/support/analyzeHeadless"

    #compile
    mkdir -p "$proj_dir/ghidra_proj" "$proj_dir/tests/output/$program"
    cmake -S "$proj_dir/sdk" -B "$proj_dir/build/tests" \
        -DCMAKE_TOOLCHAIN_FILE="$proj_dir/sdk/fr60-toolchain.cmake" \
        -DCROSS_PREFIX="$cross_prefix" \
        -DFR60_TEST_PROGRAM="$program"
    cmake --build "$proj_dir/build/tests" --target "$program"
    "${cross_prefix}objdump" -d "$program_bin_path" > "$proj_dir/tests/output/$program/objdump.S"

    #decompile
    "$analyze" "$proj_dir/ghidra_proj" fr60-test \
        -import "$program_bin_path" -overwrite \
        -processor FR:BE:32:60 \
        -cspec gcc \
        -scriptPath "$proj_dir/fr60/ghidra_scripts" \
        -preScript DisableDwarf.java \
        -preScript ImportMmio.java \
        "$proj_dir/fr60/ghidra_scripts/boards/mb91f467ba.xml" \
        -postScript ExportDecompiled.java \
        "$proj_dir/tests/output/$program/decomp.c" \
        "$proj_dir/tests/output/$program/disassembly.S" \
        "$proj_dir/tests/output/$program/p-code.txt"
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
if $build; then
    build
fi
if $test; then
    run
fi
