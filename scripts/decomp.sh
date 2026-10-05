#!/bin/bash

analyze_bin=/home/ryanw/tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless
project=fr60-test
proj_dir=/home/ryanw/cerias/ghidra-extras
program="$proj_dir"/bin/fr-30/multest.elf


while (($#)); do
    case "$1" in
        --analyze-bin|--project|--program)
            if (($# < 2)) || [[ -z "$2" || "$2" == --* ]]; then
                printf 'Missing value for %s\n' "$1" >&2
                exit 2
            fi
            case "$1" in
                --analyze-bin) analyze_bin=$2 ;;
                --project) project=$2 ;;
                --program) program=$2 ;;
            esac
            shift 2
            ;;
        *)
            printf 'Unknown argument: %s\n' "$1" >&2
            exit 2
            ;;
    esac
done

cd ~/ || exit 1

"$analyze_bin" "$HOME" "$project" \
    -import "$program" -overwrite \
    -processor FR:BE:32:60 \
    -cspec gcc \
    -scriptPath ~/cerias/ghidra-extras/scripts \
    -postScript ExportDecompiled.java "$proj_dir"/output/firmware.c