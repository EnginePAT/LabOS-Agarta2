global _start
section .text

bits 32
extern stage2_main

_start:
    call stage2_main

    ; Should never get here
    cli
    hlt
