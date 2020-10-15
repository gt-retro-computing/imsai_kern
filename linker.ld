SECTIONS
{
    . = 0x0;
    .text.init : {
      *KEEP(.text.init)
    }
    .text : {
      *(.text .text.*)
    }

    .data : {
        *(.data .rodata .data.* .rodata.*)
    }

    . = ALIGN(2);

    __bss_start = .;

    .bss : {
        *(.bss .bss.*)
    }

    __bss_end = .;

    . = ALIGN(2);
    .text_end = .;

    . = ALIGN(2);
    . += 1024;
    __stack_top = .;

    /DISCARD/ : { *(.comment) }
}

