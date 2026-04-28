global _start
section .text

bits 16
extern stage2_main
jmp short _start


CODE_SEG equ gdt_code - gdt
DATA_SEG equ gdt_data - gdt


_start:
    ; Query VESA mode info to get framebuffer address
    ; Store VBE mode info block at 0x7000 (safe scratch area)
    mov ax, 0x4F01          ; VESA get mode info
    mov cx, 0x0118          ; mode number (no linear bit here!)
    mov di, 0x7000          ; destination buffer (VBE ModeInfo block)
    int 0x10
    cmp ax, 0x004F
    jne .vesa_error

    ; Framebuffer address is at offset +40 in the ModeInfo block
    ; Store it somewhere stage2/kernel can find it
    mov eax, [0x7028]       ; 0x7000 + 0x28 = phys_base_ptr
    mov [vesa_fb], eax      ; save for later

    ; Now set the mode
    mov ax, 0x4F02
    mov bx, 0x4118
    or  bx, 0x4000
    int 0x10
    cmp ax, 0x004F
    jne .vesa_error

    ; After querying mode info, store key values
    mov ax, [0x7012]        ; width (offset +0x12)
    mov [vesa_width], ax
    mov ax, [0x7014]        ; height (offset +0x14)  
    mov [vesa_height], ax
    mov ax, [0x7010]        ; pitch/bytes per scanline (offset +0x10)
    mov [vesa_pitch], ax
    mov eax, [0x7028]       ; framebuffer address (offset +0x28)
    mov [vesa_fb], eax
    mov al, [0x7019]        ; bpp (1 byte)
    mov [vesa_bpp], al

    ; Set the size of memory map in EAX
    mov ax, 0xe801
    int 0x15
    ; ax = KB between 1MB-16MB, bx = 64KB chunks above 16MB
    ; Combine them for total extended memory
    movzx eax, ax
    shl ebx, 6          ; bx * 64 = KB above 16MB
    add eax, ebx
    add eax, 1024       ; add first 1MB
    shl eax, 10         ; convert KB to bytes
    mov [mem_size], eax
    mov [mem_size], eax

    ; Get the memory map and jump to protected mode
    call get_memory_map
    jmp start_pm
.vesa_error:
    mov si, msg_nvesa
    call print
    jmp $



print:
    mov ah, 0x0e
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret


get_memory_map:
    mov di, 0x6000
    xor ebx, ebx
    xor bp, bp
.loop:
    mov eax, 0xE820
    mov ecx, 24
    mov edx, 0x534D4150
    int 0x15
    jc .done
    cmp eax, 0x534D4150
    jne .done
    inc bp              ; count it
    add di, 24          ; advance buffer
    test ebx, ebx       ; NOW check if last entry
    jz .done
    jmp .loop
.done:
    mov [mmap_count], bp
    ret



start_pm:
    cli
    lgdt [gdtr]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp CODE_SEG:PModeMain
    jmp $

[bits 32]
PModeMain:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ebp, 0x90000
    mov esp, ebp

    ; LFramebufferInfo (bottom -> top)
    push dword [vesa_bpp]
    push dword [vesa_pitch]
    push dword [vesa_height]
    push dword [vesa_width]
    push dword [vesa_fb]

    ; LBootInfo (bottom -> top)
    push dword 0x6000
    push dword [mmap_count]
    push dword [mem_size]
    push dword 0x1000           ; Where we are loaded in memory
    push dword 0x2BADB002       ; magic

    ; jmp 0x1000
    jmp main


msg_nvesa:      db 'VESA error!', 0

vesa_width:     dd 0
vesa_height:    dd 0
vesa_pitch:     dd 0
vesa_fb:        dd 0
vesa_bpp:       dd 0

mem_size:       dd 0
mmap_count:     dd 0


gdt:
gdt_null:
    dq 0

gdt_code:
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

gdt_data:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

gdt_end:

gdtr:
    dw gdt_end - gdt - 1
    dd gdt

main:
    call stage2_main

    ; Should never get here
    cli
    hlt
