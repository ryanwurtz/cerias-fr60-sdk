#!/bin/bash

set -e

#global args
program=multest
test=false
build=false
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

    #compile
    mkdir -p "$proj_dir/tests/bin" "$proj_dir/tests/output/"{dcmp,asm,pcode}
    "${cross_prefix}gcc" -O0 -g -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000 \
        "$proj_dir/tests/src/$program.c" -lgcc -o "$program_bin_path"

    #decompile
    "$analyze" "$proj_dir/ghidra_proj" fr60-test \
        -import "$program_bin_path" -overwrite \
        -processor FR:BE:32:60 \
        -cspec gcc \
        -scriptPath "$proj_dir/ghidra_scripts" \
        -postScript ExportDecompiled.java \
        "$proj_dir/tests/output/dcmp/${program}_dcmp.c" \
        "$proj_dir/tests/output/asm/$program.S" \
        "$proj_dir/tests/output/pcode/$program.txt"
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

parse_args "$@"
if $build; then
    build
fi
if $test; then
    run
fi
