;Edited for use with open-ce
; (c) Copyright 2007-2008 Zilog, Inc.
;-------------------------------------------------------------------------
; Byte Shift Right Unsigned
; Input:
;   Operand1:
;                 A : 8 bit number to shift
;   Operand2:
;                 B : 8 bit shift count
;
; Output:
;       Result:   A : 8 bit
; Registers Used:
;       flags
;-------------------------------------------------------------------------
;        .def    __bshru
;   .assume adl=1

.section .text.lib._bshru

.globl __bshru

__bshru:
        push    bc
        inc b
        jp      .test

.loop:
        srl     a
.test:
        djnz    .loop

        pop bc
    ret

