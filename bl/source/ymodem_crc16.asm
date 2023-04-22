;-------------------------------------------------------------------------------
;  sys_core.asm
;
;  Author      : QJ Wang. qjwang@ti.com
;  Date        : 1-12-2016
;
;/* Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
;*
;*
;*  Redistribution and use in source and binary forms, with or without
;*  modification, are permitted provided that the following conditions
;*  are met:
;*
;*    Redistributions of source code must retain the above copyright
;*    notice, this list of conditions and the following disclaimer.
;*
;*    Redistributions in binary form must reproduce the above copyright
;*    notice, this list of conditions and the following disclaimer in the
;*    documentation and/or other materials provided with the
;*    distribution.
;*
;*    Neither the name of Texas Instruments Incorporated nor the names of
;*    its contributors may be used to endorse or promote products derived
;*    from this software without specific prior written permission.
;*
;*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;*/
;*****************************************************************************


    .text
    .arm


    .def     _cal_crc16_
    .asmfunc

;#define pmsg        R0;  the first argument;  R0 is also thereturn value
;#define count       R1;  the second argument
;#define msg         R2
;#define poly        R3
;#define index       R4
;#define crc         R12

_cal_crc16_

    stmfd sp!, {r2-r4, r12}   ;r2 at old_sp-4, r1 at old_sp-8, r0 at old_sp-12
                              ;sp at r0
;   unsigned short crc = 0;
	MOV             R12, #0x0
;   while(count--) {
;do 1 byte each time
	CMP             R1, #0x0           ;R1 = count,  update flags based on R1-0x0
;	SUBS            R1, R1, #1         ;R1 = R1 -1, and update the flags
	BEQ             Byte_Loop_Done

Byte_Loop_Start:
;   crc = crc ^ *buf++ << 8;
	LDRB            R2, [R0], #1            ;R0 is pmsg, R2=[R0], then R0++
	EOR             R12, R12, R2, LSL #8    ;R12 = crc

;   for (i=0; i<8; i++) {
	MOV             R4, #0x0            ;i
	CMP             R4, #0x8
	BGE             Bit_Loop_Done

Bit_Loop_Start:  ;Shift 1 bit left each time, and XOR with poly if the uppermost bit is 1
;   if (crc & 0x8000) {
	TST             R12, #0x8000      ; perform bitwise AND to check if the uppermost bit is 1
	BEQ             UpperMostBit_EZ   ; uppermost bit is Zero

;   uppermost bit is One
;   crc = (crc << 1) ^ 0x1021
	MOVW            R3,  #0x1021             ;4129, R3 is poly
	EOR             R12, R3, R12, LSL #1     ;R12 = R4 ^ (R12 << 1)
	B               Bit_Loop_End

UpperMostBit_EZ:
;   crc = crc << 1;
	MOV             R12, R12, LSL #1  ;R12= (R14 << 1), logic shift left 1 bit

Bit_Loop_End:
;   for (i=0; i<8; i++) {
	ADD             R4, R4, #1          ;r4 = i
	CMP             R4, #8
	BLT             Bit_Loop_Start

Bit_Loop_Done:
;   while(count--) {
	SUBS            R1, R1, #1     ; R1 = R1-1, update the flags
	BNE             Byte_Loop_Start

Byte_Loop_Done:
;   return crc;
    MOV             R0, R12

    ldmfd sp!, {r2-r4, r12}
	BX              lr

    .endasmfunc
