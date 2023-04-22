//*****************************************************************************
//
// sw_hw_ver.c : Funcions that are used to find the Software and the Hardware information
// Author      : QJ Wang. qjwang@ti.com
// Date        : 9-19-2012
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

/* Include Files */
#include "bl_config.h"

#if defined (UART_ENABLE_UPDATE)
#include "sys_common.h"
#include "system.h"
#include "hw_sci.h"
#include "sci_common.h"


#define DIE_ID1 		(*(volatile unsigned int *) 0xFFFFE1F0)
#define DIE_ID2 		(*(volatile unsigned int *) 0xFFFFE1F4)
#define DIE_ID3 		(*(volatile unsigned int *) 0xFFFFE1F8)
#define DIE_ID4	 	    (*(volatile unsigned int *) 0xFFFFE1FC)

//extern void sciSend_32bitdata(sciBASE_t *sci, unsigned int data);

/** @fn get_software_Version(void)
*   @brief Get the Software Version of the Demo
*/
void get_software_Version(void)
{
	UART_putString(UART, "\n\r\ The BootLoader Version: V1.0 \n\n\r");
	return;
}

/** @fn get_hardware_Info(void)
*   @brief Get the Hardware Information of the Micro
*		- Device ID
*  		- LOT Number
*  		- WAFFER Number + DIE Locations ( X & Y)
* 		- Package Type + Flash Memory 
*	@note: The above Hardware informations are stored in the OTP locations	
*/
void get_hardware_Info(void)
{
	unsigned int LOT_NUM, WAFER_LOC_NUM;
	LOT_NUM   =  ((DIE_ID1 & 0xFFC00000) >> 22) | ((DIE_ID2 & 0x00003FFF) << 10);
	WAFER_LOC_NUM = (DIE_ID1 & 0x003FFFFF);
	
	UART_putString(UART, "\n\r Device Information: \r\n\ ");

	UART_putString(UART, "DEV:  ");
	UART_send32BitData(UART, systemREG1->DEV);
	UART_putString(UART, "  \r\n ");
	UART_putString(UART, "LOT NUM:  ");
	UART_send32BitData(UART, LOT_NUM);
	UART_putString(UART, " \r\n ");
	UART_putString(UART, "WAFER LOC NUM:  ");
	UART_send32BitData(UART, (WAFER_LOC_NUM));
	UART_putString(UART, " \n\n\r\n ");

	return;
}
#endif
