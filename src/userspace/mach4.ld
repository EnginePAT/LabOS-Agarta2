ENTRY(_start)

SECTIONS {
    . = 0;

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
