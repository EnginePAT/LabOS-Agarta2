global _start
section .text

[bits 32]
extern kernel_main

_start:
    ; Execute the kernel
    call kernel_main
    
    ; We should never get here
    cli
    hlt
    jmp $
