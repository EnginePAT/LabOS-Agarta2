global jump_usermode

jump_usermode:
    mov ax, 0x23                ; User data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; iret frame: SS, ESP, eflags, CS, EIP
    push 0x23                   ; User stack segment
    push 0xBFFFFFF0             ; User ESP (top of user stack)
    pushf
    pop eax
    or eax, 0x200               ; Enable interrupts in eflags
    push eax
    push 0x1B                   ; User code segment (GDT index 3, RPL 3)
    push usermode_test          ; EIP
    iret

usermode_test:
    ; We are now in ring 3!
    ; Can't call kernel functions directly anymore
    jmp $
