ENTRY(_start)

MEMORY
{
    RAM   (rw)  : ORIGIN = 0x0002A000, LENGTH = 24K
    IDRAM (rwx) : ORIGIN = 0x00030000, LENGTH = 16K
    FLASH (rx) : ORIGIN = 0x00040000, LENGTH = 1088K
}

STACK_SIZE = 4K;
HEAP_SIZE = 4K;

SECTIONS
{   
    .startup 0x000F4000 : ALIGN(4) {
        KEEP(*(.text.boot .text.boot.*))
        KEEP(*(.startup .startup.*))
    } > FLASH

    .text 0x00040000 : ALIGN(4) { 
        *(.text .text.* .gnu.linkonce.t.*)
    } > FLASH

    .rodata : ALIGN(4) {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    } > FLASH

    .data : ALIGN(4) {
        __data_start = .;
        *(.data .data.* .gnu.linkonce.d.*)
        __data_end = .;
    } > RAM AT> FLASH

    __data_load_start = LOADADDR(.data);

    ASSERT(LOADADDR(.data) + SIZEOF(.data) <= ADDR(.startup),
       "FLASH overflow: code/data image reaches startup at 0xF4000")
    ASSERT(DEFINED(_start) ? _start == ADDR(.startup) : 1,
       "_start must be at the beginning of startup at 0xF4000")

    .bss (NOLOAD) : ALIGN(4) {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    } > RAM

    .heap (NOLOAD) : ALIGN(8) {
        __heap_start = .;
        . += HEAP_SIZE;
        __heap_limit = .;
    } > RAM

    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
    __stack_bottom = __stack_top - STACK_SIZE;

    .stack __stack_bottom (NOLOAD) : {
        . += STACK_SIZE;
    } > RAM

    ASSERT(__heap_limit <= __stack_bottom,
       "RAM overflow: heap overlaps stack")

   /DISCARD/ : { *(.comment) *(.debug*) }
}
