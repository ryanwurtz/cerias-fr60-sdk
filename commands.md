fr30-elf-gcc -O0 -g -nostdlib -Wl,-e,main -Wl,-Ttext=0x10000 src/fr-30/divtest.c -lgcc -o bin/fr-30/divtest.elf
fr30-elf-objdump -d --disassemble=__divsi3 bin/fr-30/divtest.elf
fr30-elf-gcc -O0 -S src/fr-30/divtest.c -o asm/fr-30/divtest.S