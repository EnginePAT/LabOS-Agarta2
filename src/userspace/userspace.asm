[bits 32]
global jump_usermode
global usermode_test

jump_usermode:
    mov ecx, [esp+4]      ; entry  → ecx (safe, not touched below)
    mov ebx, [esp+8]      ; user_stack

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword 0x23       ; SS
    push ebx              ; ESP
    pushfd
    or dword [esp], 0x200
    push dword 0x1B       ; CS
    push ecx              ; EIP  ← was eax, now ecx
    iretd

usermode_test:
    jmp $
