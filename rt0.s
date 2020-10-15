.section .text.init

.globl _start

_start:
    ld hl, __stack_top
    ld sp, hl
    CALL _initialize_uart
    CALL _c_start
_dead:
    ld a, l
    cpl
    out (0xFF), a
    halt
    jp _dead

_initialize_uart:
    ld a, 0xc0
    out (0x10), a
    out (0x20), a
    ld a, 0x1
    out (0x12), a
    out (0x22), a

    ret

; Library Functions
