ENTRY(kernel_main)
SECTIONS {
    . = 0x100000;
    _userland_start = .;

    .text   : { *(.text)   }
    .rodata : { *(.rodata) }
    .data   : { *(.data)   }
    .bss    : { *(.bss) *(COMMON) }

    _userland_end = .;
}
