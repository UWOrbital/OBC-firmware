;-------------------------------------------------------------------------------
;  sys_core.asm
;
;  Author      : QJ Wang. qjwang@ti.com
;  Date        : 1-12-2016
;
;* Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
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
;;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;;  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;*/
; *****************************************************************************

    .text
    .arm
;-------------------------------------------------------------------------------
;
; Copy the Flash API from flash to SRAM.
;
    .def     _copyAPI2RAM_
    .asmfunc

_copyAPI2RAM_

     add    r2, r1, r2
copy_loop:
     ldr     r3, [r0], #4
     str     r3, [r1], #4
     cmp     r1, r2
     blt     copy_loop
      bx     lr

    .endasmfunc


;-------------------------------------------------------------------------------

