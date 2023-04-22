//*****************************************************************************
//
/* Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//*****************************************************************************


#ifndef __BL_FLASH_H__
#define __BL_FLASH_H__

#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_sys_common.h"
#else
#include "sys_common.h"
#endif


//*****************************************************************************
//
// Basic functions for erasing and programming internal flash.
//
//*****************************************************************************
extern uint32_t BLInternalFlashFirstSectorSizeGet(void);
extern uint32_t BLInternalFlashSizeGet(void);
extern uint32_t BLInternalFlashStartAddrCheck(uint32_t ulAddr, uint32_t ulImgSize);

extern uint32_t Fapi_Init(uint32_t ucStartBank, uint32_t ucEndBank);
extern uint32_t Fapi_BlockProgram( uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes);

extern uint32_t Fapi_BlockErase( uint32_t Flash_Start_Address, uint32_t Size_In_Bytes);

extern uint32_t Fapi_UpdateStatusProgram( uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes);
                                
extern uint32_t Flash_Erase_Check(uint32_t Start_Address, uint32_t Bytes);

extern uint32_t Flash_Program_Check(uint32_t Program_Start_Address, uint32_t Source_Start_Address, uint32_t No_Of_Bytes);

#endif // __BL_FLASH_H__
