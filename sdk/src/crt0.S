    .section .text.boot,"ax",@progbits
    .balign 4
    .global _start
    .type _start, @function

_start:
    andccr #0xcf
    ldi:32 #__stack_top, r15
    ldi:32 #__data_load_start, r0
    ldi:32 #__data_start, r1
    ldi:32 #__data_end, r2
.Lcopy_data:
    cmp r2, r1
    beq .Lclear_setup
    ldub @r0, r3
    stb r3, @r1
    add #1, r0
    add #1, r1
    bra .Lcopy_data

.Lclear_setup:
    ldi:32 #__bss_start, r1
    ldi:32 #__bss_end, r2
    ldi:8 #0, r3
.Lclear_bss:
    cmp r2, r1
    beq .Lmain
    stb r3, @r1
    add #1, r1
    bra .Lclear_bss

.Lmain:
    ldi:8 #0, r4
    ldi:8 #0, r5
    ldi:32 #main, r0
    call @r0
.Lhalt:
    bra .Lhalt

    .size _start, .-_start
