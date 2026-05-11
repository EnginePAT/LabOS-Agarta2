global context_switch
; void context_switch(uint32_t* old_esp, uint32_t new_esp, uint32_t new_cr3)
context_switch:
    ; Save current process state
    push ebp
    push ebx
    push esi
    push edi

    ; Save old ESP
    mov eax, [esp + 20]     ; old_esp argument
    mov [eax], esp

    ; Load the new ESP
    mov esp, [esp + 24]     ; new_esp argument

    ; Switch page directory if needed
    ; mov eax, [esp + 28]     ; new_cr3 argument
    ; mov cr3, eax

    ; Restore new process state
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
