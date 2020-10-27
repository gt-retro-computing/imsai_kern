; (c) Copyright 2001-2008 Zilog, Inc.
;-------------------------------------------------------------------------
; Short Division Unsigned
; Input:
;   Operand1:
;         hl : 16 bits
;
;   Operand2:
;         bc : 16 bits
;
; Output:
;   Result:   hl : 16 bits
;
; Registers Used:
;
;-------------------------------------------------------------------------
.section .text.lib._sdivu

.globl __sdivu

__sdivu:
    push af
    push bc
    push de

    push bc
    pop de

    call my_udiv

    pop de
    pop bc
    pop af
    ret

    ; HL = dividend
    ; DE = divisor

my_udiv:
    ; check for division by zero
    ld a, e
    or d
    jp nz, .divide
    ; error handling, we divide by zero
    scf
    ret

.divide:
    ld c, l
    ld a, h
    ld hl, 0
    ld b, 16
    or a
.dvloop:
    rl c
    rla
    rl l
    rl h

    push hl
    sbc hl, de
    ccf

    jp c, .drop
    ex (sp), hl
.drop:
    inc sp
    inc sp
    djnz .dvloop

    ex de, hl
    rl c
    ld l, c
    rla
    ld h, a
    or a
    ret



