;Edited for use with open-ce
; (c) Copyright 2007-2008 Zilog, Inc.
;-------------------------------------------------------------------------
; Unsigned Byte Multiply
; Input:
;   Operand1:
;         B : 8 bit
;
;   Operand2:
;         C : 8 bit
;
; Output:
;   Result:   A : 8 bit product
; Registers Used:
;   none
;-------------------------------------------------------------------------

.section .text.lib._bmulu

.globl __bmulu

__bmulu:
    push bc
    push de

    ld d, b
    ld b, 8
    xor a
.loop:
    sla a
    rlc c
    jp nc, .noadd
    add a, d
.noadd:
    djnz .loop

    pop de
    pop bc
    ret
