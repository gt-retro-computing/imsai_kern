.section .text.lib._memset

    .globl  _memset                         ; -- Begin function memset
; void *memset(void *dest, char c, int count); ** LLVM MEM SET
; a = "c"
_memset:
    ld  iy,0
    add iy,sp
    ld  e, (iy+2)
    ld  d, (iy+3)
    ld  l, (iy+4)
    ld  h, (iy+5)

    ld  bc,0
    or a
    sbc hl,bc
    jp z,.exit
    ld (de),a
    inc bc
    or a
    sbc hl,bc
    jp z,.exit
    push hl
    pop bc
    push de
    inc de
    pop hl
    ldir
.exit:
    ld  l, (iy+4)
    ld  h, (iy+5)
    ret
