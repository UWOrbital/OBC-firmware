/*****************************************************************************
 *
 * bl_flash.c     : The file holds the main control loop of the boot loader.
 * Author         : QJ Wang. qjwang@ti.com
 * Date           : 4-19-2019
 */
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

#include "bl_config.h"
#include "bl_flash.h"
#include "F021.h"
#include "flash_defines.h"


//#define Freq_In_MHz = SYS_CLK_FREQ;
uint8_t    g_ucStartBank, g_ucEndBank, g_ucStartSector, g_ucEndSector;
uint32_t   g_ulBankInitialized = 0;

//*****************************************************************************
//
// Returns the size of the ist sector size of the flash in bytes.
//
//*****************************************************************************
uint32_t
BLInternalFlashFirstSectorSizeGet(void)
{
	uint32_t firstSectorSize;
	firstSectorSize = (uint32_t)(flash_sector[0].start) + flash_sector[0].length;
    return (firstSectorSize);
}
//*****************************************************************************
//
// Returns the size of the internal flash in bytes.
//
// This function returns the total number of bytes of internal flash in the
// current part.  No adjustment is made for any sections reserved via
// options defined in bl_config.h.
//
// \return Returns the total number of bytes of internal flash.
//
//*****************************************************************************
uint32_t
BLInternalFlashSizeGet(void)
{
	uint32_t flashSize;
	flashSize = (uint32_t)flash_sector[NUMBEROFSECTORS-1].start + flash_sector[NUMBEROFSECTORS-1].length;
    return (flashSize);
}

//*****************************************************************************
//
// Checks whether a given start address is valid for a download.
//
// This function checks to determine whether the given address is a valid
// download image start address given the options defined in bl_config.h.
//
// \return Returns non-zero if the address is valid or 0 otherwise.
//
//*****************************************************************************
uint32_t
BLInternalFlashStartAddrCheck(uint32_t ulAddr, uint32_t ulImgSize)
{
    uint32_t count=0, i;

	uint32_t ulWholeFlashSize;

    //
    // Determine the size of the flash available on the part in use.
    //
    ulWholeFlashSize = (uint32_t)flash_sector[NUMBEROFSECTORS-1].start + flash_sector[NUMBEROFSECTORS-1].length;

	/* The start address must be at the beginning of the sector */
    for (i = 0; i < NUMBEROFSECTORS; i++){
		if ((ulAddr >= (uint32_t)(flash_sector[i].start)) && (ulAddr < ((uint32_t)flash_sector[i].start + flash_sector[i].length)))
		{
			count++;
		}
	}
    if (count == 0){
    	return(0);
    }

    //
    // Is the address we were passed a valid start address?  We allow:
    //
    // 1. Address 0 if configured to update the boot loader.
    // 2. The start of the reserved block if parameter space is reserved (to
    //    allow a download of the parameter block contents).
    // 3. The application start address specified in bl_config.h.
    //
    // The function fails if the address is not one of these, if the image
    // size is larger than the available space or if the address is not word
    // aligned.
    //
    if((
#ifdef ENABLE_BL_UPDATE
                       (ulAddr != 0) &&
#endif
                        (ulAddr != APP_START_ADDRESS)) ||
                       ((ulAddr + ulImgSize) > ulWholeFlashSize) ||
                       ((ulAddr & 3) != 0))
    {
    	return(0);
    }
    else  {
        return(1);
    }
}

/* Initialize the flash bank and activate the sectors */
uint32_t Fapi_Init(uint32_t ucStartBank, uint32_t ucEndBank)
{
    uint32_t i;

    /*SYS_CLK_FREQ is defined in bl_config.h */
    if ((Fapi_initializeFlashBanks((uint32_t)SYS_CLK_FREQ)) == Fapi_Status_Success){
        for (i = ucStartBank; i < ( ucEndBank + 1); i++){
            (void)Fapi_setActiveFlashBank((Fapi_FlashBankType)i);
            (void)Fapi_enableMainBankSectors(0xFFFF);                    /* used for API 2.01*/
            while( FAPI_CHECK_FSM_READY_BUSY != Fapi_Status_FsmReady );
            while( FAPI_GET_FSM_STATUS != Fapi_Status_Success ); /* don't have to include this one*/
        }
        g_ulBankInitialized = 1;

    }else {
         return (1);
    }
    return (0); //success
}

/* ulAddr must be starting address of one flash sector*/
uint32_t Fapi_BlockErase(uint32_t ulAddr, uint32_t Size)
{
	uint8_t  i=0, ucStartSector, ucEndSector;
    uint32_t EndAddr, status;

	EndAddr = ulAddr + Size;
	for (i = 0; i < NUMBEROFSECTORS; i++){
		if (ulAddr < (uint32_t)(flash_sector[i].start))
		{
			g_ucStartBank     = flash_sector[i-1].bankNumber;
		    ucStartSector   = i-1;
		    break;
		}
	}

	for (i = ucStartSector; i < NUMBEROFSECTORS; i++){
		if (EndAddr <= (((uint32_t)flash_sector[i].start) + flash_sector[i].length))
		{
			g_ucEndBank   = flash_sector[i].bankNumber;
			ucEndSector = i;
		    break;
		}
	}

    status = Fapi_Init(g_ucStartBank, g_ucEndBank);

    for (i=ucStartSector; i<(ucEndSector+1); i++){
		Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, flash_sector[i].start);
    	while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
    	while(FAPI_GET_FSM_STATUS != Fapi_Status_Success);
    }

#if defined (RM57) || defined (TMS570LC43)
    status =  0;
#else
    status =  Flash_Erase_Check((uint32_t)ulAddr, Size);
#endif

	return (status);
}

//Bank here is not used. We calculate the bank in the function based on the Flash-Start-addr
uint32_t Fapi_BlockProgram(uint32_t Flash_Address, uint32_t Data_Address, uint32_t SizeInBytes)
{
	register uint32_t src = Data_Address;
	register uint32_t dst = Flash_Address;
	uint32_t bytes;

	if (SizeInBytes < 16)
		bytes = SizeInBytes;
	else
		bytes = 16;

    /* The flash bank has been initialized in flash erase function */
    if (g_ulBankInitialized |= 1 ){
        Fapi_Init(g_ucStartBank, g_ucEndBank);
    }

    while( SizeInBytes > 0)
	{
		Fapi_issueProgrammingCommand((uint32_t *)dst,
									 (uint8_t *)src,
									 (uint32_t) bytes,
									 0,
									 0,
									 Fapi_AutoEccGeneration);

 		while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
        while(FAPI_GET_FSM_STATUS != Fapi_Status_Success);

		src += bytes;
		dst += bytes;
		SizeInBytes -= bytes;
        if ( SizeInBytes < 32){
           bytes = SizeInBytes;
        }
    }
	return (0);
}


uint32_t Fapi_UpdateStatusProgram(uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	unsigned int bytes;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;

	Fapi_issueProgrammingCommand((uint32_t *)dst,
									 (uint8_t *)src,
									 (uint32_t) bytes,
									 0,
									 0,
									 Fapi_AutoEccGeneration);

 	while( Fapi_checkFsmForReady() == Fapi_Status_FsmBusy );
    while(FAPI_GET_FSM_STATUS != Fapi_Status_Success);

	return (0);
}



uint32_t Flash_Program_Check(uint32_t Program_Start_Address, uint32_t Source_Start_Address, uint32_t No_Of_Bytes)
{
	register uint32_t *src1 = (uint32_t *) Source_Start_Address;
	register uint32_t *dst1 = (uint32_t *) Program_Start_Address;
	register uint32_t bytes = No_Of_Bytes;

	while(bytes > 0)
	{	
		if(*dst1++ != *src1++)
			return (1);   //error

		bytes -= 0x4;
	}
	return(0);
}	


uint32_t Flash_Erase_Check(uint32_t Start_Address, uint32_t Bytes)
{
	uint32_t error=0;
	register uint32_t *dst1 = (uint32_t *) Start_Address;
	register uint32_t bytes = Bytes;

	while(bytes > 0)
	{	
		if(*dst1++ != 0xFFFFFFFF){
			error = 2;
		}
		bytes -= 0x4;
	}
	return(error);
}



uint32_t Fapi_BlockRead( uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	register uint32_t bytes_remain = Size_In_Bytes;
	int bytes;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;

 	while( bytes_remain > 0)
	{
		Fapi_doMarginReadByByte((uint8_t *)src,
								(uint8_t *)dst,
								(uint32_t) bytes,                //16
								Fapi_NormalRead);
		src += bytes;
		dst += bytes;
        bytes_remain -= bytes;
    }
	return (0);
}
