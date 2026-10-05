#!/bin/bash

#program parsing
proj_dir=/home/ryanw/cerias/ghidra-extras
program=multest
while (($#)); do
    case "$1" in
        --program)
            if (($# < 2)) || [[ -z "$2" || "$2" == --* ]]; then
                printf 'Missing value for %s\n' "$1" >&2
                exit 2
            fi
            program=$2
            shift 2
            ;;
        *)
            printf 'Unknown argument: %s\n' "$1" >&2
            exit 2
            ;;
    esac
done

#compile
program_path="$proj_dir/bin/fr-30/$program.elf"
fr30-elf-gcc -O0 -g -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000 \
    "$proj_dir/src/fr-30/$program.c" -lgcc -o "$program_path"

#decompile
/home/ryanw/tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless "$HOME" fr60-test \
    -import "$program_path" -overwrite \
    -processor FR:BE:32:60 \
    -cspec gcc \
    -scriptPath ~/cerias/ghidra-extras/scripts \
    -postScript ExportDecompiled.java "$proj_dir"/output/"$program"_dcmp.c

exit 0