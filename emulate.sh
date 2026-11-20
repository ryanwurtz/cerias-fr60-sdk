#!/bin/bash
set -euo pipefail

usage() {
    echo "Usage: $0 PROGRAM [--trace] [--max-instructions N]"
    echo "Run firmware previously built with ./run.sh -p PROGRAM --startup."
    echo "USART0 reads stdin and writes stdout. Press Enter to send a terminal line."
    echo "Press Ctrl-C to stop a program waiting for more input."
}

case "${1:-}" in
    -h|--help) usage; exit 0 ;;
esac
if [[ ! "${1:-}" =~ ^[A-Za-z0-9_]+$ ]]; then
    usage >&2
    exit 2
fi

program=$1
shift
runner_args=(--uart-terminal)
max_instructions=18446744073709551615
while (($#)); do
    case "$1" in
        --trace)
            runner_args+=(--trace)
            shift
            ;;
        --max-instructions)
            if (($# < 2)) || [[ ! "$2" =~ ^[0-9]+$ || ! "$2" =~ [1-9] ]]; then
                printf 'Expected a positive instruction budget after --max-instructions\n' >&2
                exit 2
            fi
            max_instructions=$2
            shift 2
            ;;
        *)
            printf 'Unknown argument: %s\n' "$1" >&2
            exit 2
            ;;
    esac
done

proj_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cross_prefix=${CROSS_PREFIX:-$proj_dir/.tools/cross/bin/fr30-elf-}
elf="$proj_dir/tests/output/$program/$program.elf"
bin="$proj_dir/tests/output/$program/$program.bin"
for artifact in "$elf" "$bin"; do
    if [[ ! -f "$artifact" ]]; then
        printf 'Missing artifact: %s. Build it with ./run.sh -p %s --startup.\n' "$artifact" "$program" >&2
        exit 1
    fi
done

entry=$("${cross_prefix}objdump" -f "$elf" | awk '/start address/ {print $3}')
stop=$("${cross_prefix}nm" --defined-only "$elf" | awk '$3 == "__emulator_exit" {print "0x" $1}')
if [[ ! "$entry" =~ ^0x[[:xdigit:]]+$ || ! "$stop" =~ ^0x[[:xdigit:]]+$ ]]; then
    echo 'Could not resolve ELF entry or __emulator_exit. Build the program with --startup.' >&2
    exit 1
fi

cd -- "$proj_dir/styx-emulator"
exec cargo run --quiet --locked -p fr60-runner -- --bin "$bin" --base 0x40000 \
    --entry "$entry" --sp 0x30000 --stop "$stop" --max-instructions "$max_instructions" \
    "${runner_args[@]}"
