ENTRY(_start)

SECTIONS {
    . = 0x00400000;

    .mach4hdr : {
        *(.mach4hdr)
    }

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
    }
}
