```bash
mkdir -p tests/bin tests/output/asm
.tools/cross/bin/fr30-elf-gcc -O0 -g -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000 \
    tests/src/divtest.c -lgcc -o tests/bin/divtest.elf
.tools/cross/bin/fr30-elf-objdump -d --disassemble=__divsi3 tests/bin/divtest.elf
.tools/cross/bin/fr30-elf-gcc -O0 -S tests/src/divtest.c -o tests/output/asm/divtest.S
```

`run.sh` defaults to `.tools/ghidra` and `.tools/cross/bin/fr30-elf-`. Override them with `GHIDRA_INSTALL_DIR` and `CROSS_PREFIX` when needed. Extension ZIPs are written to `fr60/dist/`; the module is installed in Ghidra's user extension directory under `${XDG_CONFIG_HOME:-$HOME/.config}/ghidra/`.