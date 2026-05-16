; userland.asm
BITS 32
GLOBAL _start
EXTERN main

; --- Mach4 header ---
jmp short _start
nop

SECTION .mach4hdr

    dd 0x4D344558
    db 1
    db 0
    dd _start - $$     ; entry offset
    dd 0
    dd 0
    dw 0

; --- Entry point ---
SECTION .text
_start:
    call main
    jmp $