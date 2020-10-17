;Edited for use with open-ce
; (c) Copyright 2001-2008 Zilog, Inc.
;-------------------------------------------------------------------------
; Short Shift Right Signed
; Input:
;   Operand1:
;         BC : 16 bit number of bits to shift
;           Assume B=0 C=shift count
;   Operand2:
;         HL : 16 bit value to be shifted by C
;
; Output:
;   Result:   HL : 16 bit
; Registers Used:
;   none
;-------------------------------------------------------------------------
;   .def    __sshrs, __sshrs_b
;   .assume adl=1

.section .text.lib._sshrs

.globl __sshrs
.globl __sshrs_b

__sshrs_b:
    push bc
    ld c,a
    call __sshrs
    pop bc
    ret

__sshrs:
    push    bc
    ld  b,c
    inc b
    jp  .test
.loop:
    sra h
    rr  l
.test:
    djnz    .loop
    pop bc
    ret

