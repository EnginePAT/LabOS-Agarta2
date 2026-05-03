extern syscall_handler

global syscall_interrupt_handler
syscall_interrupt_handler:
    pushad                  ; Push all general purpose (GP) registers (EAX, EBX etc...)
    push esp                ; Push the pointer to the registers as an argument

    call syscall_handler
    add esp, 4              ; Clean up the stack

    ; The return value from syscall_handler is in EAX
    ; We must ensure we don't overwrite it when restoring pushad
    mov [esp + 28], eax     ; Overwrite the saved EAX on the stack with the return value
    popad                   ; Restore registers (EAX now contains return value)
    iret
