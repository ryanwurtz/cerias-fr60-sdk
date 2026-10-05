#!/bin/bash

set -e

#global args
program=multest
build=false
proj_dir=$HOME/cerias/cerias-fr60-ghidra
ghidra_dir=${GHIDRA_INSTALL_DIR:-$HOME/Downloads/ghidra_12.1.3_PUBLIC_20260817/ghidra_12.1.3_PUBLIC}

parse_args() {
    while (($#)); do
        case "$1" in
            -p)
                if (($# < 2)) || [[ -z "$2" || "$2" == --* ]]; then
                    printf 'Missing value for %s\n' "$1" >&2
                    exit 2
                fi
                program=$2
                shift 2
                ;;
            -b)
                build=true
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
    local program_bin_path="$proj_dir/tests/bin/$program.elf"
    local analyze="$ghidra_dir/support/analyzeHeadless"
    #local analyze=/$HOME/tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless 

    #compile
    fr30-elf-gcc -O0 -g -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000 \
        "$proj_dir/tests/src/$program.c" -lgcc -o "$program_bin_path"

    #decompile
    "$analyze" "$HOME" fr60-test \
        -import "$program_bin_path" -overwrite \
        -processor FR:BE:32:60 \
        -cspec gcc \
        -scriptPath "$proj_dir/ghidra_scripts" \
        -postScript ExportDecompiled.java "$proj_dir"/tests/output/"$program"_dcmp.c
}

build() {
    local extension_dir="$proj_dir/fr60"
    local install_dir="$ghidra_dir/Ghidra/Extensions"
    local archive

    (
        cd -- "$extension_dir/data"
        "$ghidra_dir/support/sleigh" -i sleighArgs.txt -a languages
    )

    "$ghidra_dir/support/gradle/gradlew" -p "$extension_dir" \
        -PGHIDRA_INSTALL_DIR="$ghidra_dir" clean buildExtension --rerun-tasks

    #install most recent build
    archive=$(ls -t "$extension_dir"/dist/*.zip | head -n 1)
    unzip -o "$archive" -d "$install_dir"
}

parse_args "$@"
if $build; then
    build
fi
run
