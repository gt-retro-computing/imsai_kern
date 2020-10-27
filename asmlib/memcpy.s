; void *memcpy( void *dest, const void *src, size_t count );

.section .text.lib._memcpy
.globl _memcpy

_memcpy:
    ld  iy,0
    add iy,sp
    ld  c, (iy+6)
    ld  b, (iy+7) ; BC = count

    sbc hl,hl
    sbc hl,bc   ;count==0?

    jp  z, .L0
    ld  e, (iy+2)
    ld  d, (iy+3)   ; DE = DST
    ld  l, (iy+4)
    ld  h, (iy+5)   ; HL = SRC
    ldir
.L0:
    ld  l, (iy+2)
    ld  h, (iy+3)   ; HL = DST
    ret
