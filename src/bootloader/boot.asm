org 0x7c00
bits 16

;
; LabFS-Lite Superblock
;
jmp short _start
nop

magic:          dd 0x4fb6
version:        db 1
resv:           db 1
block_size:     dd 512
inode_start:    dd 1
data_start:     dd 2
inode_count:    dd 2
root_inode:     dd 0
reserved:       dw 0

_start:
    mov [BOOT_DRIVE], dl

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7bff

    jmp main

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

;
; read_sector: ax = LBA sector, bx = destination
;
read_sector:
    push ax
    push bx
    push cx
    push dx

    mov cl, al
    inc cl              ; BIOS sectors are 1-indexed
    mov ah, 0x02
    mov al, 1
    mov ch, 0
    mov dh, 0
    mov dl, 0x80        ; first drive
    int 0x13
    jc .disk_error

    pop dx
    pop cx
    pop bx
    pop ax
    ret
.disk_error:
    mov si, msg_diskerr
    call print
    jmp $

main:
    ; Load inode table into 0x7E00
    mov al, [inode_start]
    xor ah, ah
    mov bx, 0x7E00
    call read_sector

    ; Load directory entries into 0x7F00
    mov al, [data_start]
    xor ah, ah
    mov bx, 0x7F00
    call read_sector

    ; Scan directory entries for "stage2.bin"
    mov di, 0x7F00

    ; Entry 0
    mov si, stage2_name
    mov cx, 10
    push di
    repe cmpsb
    pop di
    je .found

    ; Entry 1
    add di, 60
    mov si, stage2_name
    mov cx, 10
    push di
    repe cmpsb
    pop di
    je .found

    ; Entry 2
    add di, 60
    mov si, stage2_name
    mov cx, 10
    push di
    repe cmpsb
    pop di
    je .found

    jmp .not_found

.found:
    mov si, msg_found
    call print

    ; Get inode_idx from DirEntry (+56)
    mov ax, [di + 56]   ; use ax not eax!
    xor dx, dx          ; clear dx before mul
    mov bx, 16
    mul bx              ; ax = inode_idx * 16
    add ax, 0x7E00      ; ax = address of inode in memory
    mov si, ax

    ; Read start_block and block_count from inode
    mov ax, [si + 8]    ; start_block
    mov cx, [si + 12]   ; block_count

    ; Load cx sectors into 0x1000
    mov bx, 0x1000
.load_loop:
    push cx
    push ax
    call read_sector
    pop ax
    pop cx
    add ax, 1
    add bx, 512
    loop .load_loop

    ; Switch to protected mode and jump to stage2
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    jmp start_pm

.not_found:
    mov si, msg_nfound
    call print
    jmp $

msg_found:      db 'Found stage2!', 0
msg_nfound:     db 'Could not find stage2!', 0
msg_diskerr:    db 'Disk error!', 0
BOOT_DRIVE:     db 0
stage2_name:    db 'stage2.bin', 0

CODE_SEG equ gdt_code - gdt
DATA_SEG equ gdt_data - gdt

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

    push dword 0x1000           ; addr
    push dword 0x2BADB002       ; magic
    call 0x1000

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

times 510-($-$$) db 0
dw 0xaa55
