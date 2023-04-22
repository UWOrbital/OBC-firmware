/********************************************************************************************************
*
* bl_main.c      : The main filefor the boot loader.
* Author         : QJ Wang. qjwang@ti.com
* Date           : 5-25-2019
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
/*---------------------------------------------------------------------------------------------------------------------
* Revision History
*---------------------------------------------------------------------------------------------------------------------
* Version          Date        Author         Description
*---------------------------------------------------------------------------------------------------------------------
* 00.10.00       19/09/2012    QJ Wang        1. Initial Version
* 00.10.01       10/01/2014    QJ Wang        1. Changes for Rev2.01.00 F021 Flash API
* 00.10.02       11/12/2014    QJ Wang        1. Changes for Rev2.01.01 F021 Flash API
* 00.20.00       18/02/2016    QJ Wang        1. Rewrote code for y-modem protocol, and CRC16
*                                             2. Changes in bl_flash.c, bl_link.cmd
*                                             3. Changed App start address to 0x10100, The 0x10000~0x10100 is left for
*                                                application update status
* 00.30.00       05/25/2019    QJ Wang        1. Changes in bl_flash.c, bl_link.cmd
*                                             2. Changed App start address to 0x10020, The 0x10000~0x10020 is left for
*                                                application update status
*                                             3. TMS570LC43x, RM57Lx, TMS570LS07x and RM44Lx devices are supported
*                                             4. Linker ECC to generate ECC for the whole flash
*                                             5. Copy the const to SRAM
*                                             6. Use HALCoGen driver for initialization
***********************************************************************************************************/

#include "bl_config.h"
#include "sci_common.h"

#if defined (CAN_ENABLE_UPDATE)
#include "bl_can.h"
#endif

#if defined (SPI_ENABLE_UPDATE)
#include "bl_spi.h"
#endif

#if defined (UART_ENABLE_UPDATE)
#include "bl_uart.h"
#endif

#if defined (RM57) || defined (TMS570LC43)
#include "HL_het.h"
#include "HL_gio.h"
#include "HL_sci.h"
#include "HL_system.h"
#include "HL_sys_common.h"
#include "HL_sys_core.h"
#else
#include "het.h"
#include "sci.h"
#include "gio.h"
#include "system.h"
#include "sys_common.h"
#include "sys_core.h"
#endif

#include "bl_check.h"
#include "bl_led_demo.h"

/*****************************************************************************
* bl_main
******************************************************************************/
#if defined (SPI_ENABLE_UPDATE) || defined(UART_ENABLE_UPDATE) || defined(CAN_ENABLE_UPDATE)

/*****************************************************************************
*
* This holds the current remaining size in bytes to be downloaded.
*
******************************************************************************/
uint32_t g_ulTransferSize;

/*****************************************************************************
*
* This holds the current address that is being written to during a download
* command.
*
******************************************************************************/
uint32_t g_ulTransferAddress;

/*****************************************************************************
*
* This is the data buffer used during transfers to the boot loader.
*
******************************************************************************/
uint32_t g_pulDataBuffer[BUFFER_SIZE];

/*****************************************************************************
*
* This is the data buffer used for update status.
*
* g_pulUpdateSuccess[] are used to store application update status and application
* image's version etc
******************************************************************************/

uint32_t g_pulUpdateSuccess[8] = {0x5A5A5A5A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint32_t g_ulUpdateStatusAddr = APP_STATUS_ADDRESS;

uint32_t g_ulUpdateBufferSize = 32;    /*16 bytes or 4 32-bit words*/

#define E_PASS     		0
#define E_FAIL     		0x1U
#define E_TIMEOUT  		0x2U

/*****************************************************************************
*
* This is an specially aligned buffer pointer to g_pulDataBuffer to make
* copying to the buffer simpler.  It must be offset to end on an address that
* ends with 3.
*
******************************************************************************/
uint8_t *g_pucDataBuffer;

extern unsigned int apiLoadStart;
extern unsigned int apiLoadSize;
extern unsigned int apiRunStart;


extern unsigned int constLoadStart;
extern unsigned int constLoadSize;
extern unsigned int constRunStart;


/*****************************************************************************
*
* This holds the current address that is being written to during a download
* command.
*
******************************************************************************/
void delay(unsigned int delayval) {
	  while(delayval--);
}

//extern void _copyAPI2RAM_(unsigned int* r0, unsigned int* r1, unsigned int* r2);

void main(void)

{
    g_pulUpdateSuccess[3] = 0x30002019;  /*version number, 03.00, in 2019*/
    uint32_t fnRetValue = 0;

    /* Initialize SCI Routines to receive Command and transmit data */
	sciInit();

    /* Copy the flash APIs to SRAM*/
	//_copyAPI2RAM_(&apiLoadStart, &apiRunStart, &apiLoadSize);
    memcpy(&apiRunStart, &apiLoadStart, (uint32)&apiLoadSize);

    /* Copy the .const section */
    //_copyAPI2RAM_(&constLoadStart, &constRunStart, &constLoadSize);
	memcpy(&constRunStart, &constLoadStart, (uint32)&constLoadSize);


#if defined (SPI_ENABLE_UPDATE)
	UART_putString(UART, "\r Hercules MCU SPI BootLoader ");
#endif
#if defined (CAN_ENABLE_UPDATE)
	UART_putString(UART, "\r Hercules MCU CAN BootLoader ");
#endif
#if defined (UART_ENABLE_UPDATE)
	UART_putString(UART, "\r Hercules MCU UART BootLoader ");
#endif

	UART_putString(UART, "\r TI DSP Application Team, qjwang@ti.com \r\r");

	//
	//  See if an update should be performed.
	//
    fnRetValue = CheckForceUpdate();

    //fnRetValue = 0;  //qj for testing application code

    if ( !fnRetValue )
    {
#ifdef DEBUG_MSG
    	UART_putString(UART, "\r Jump to application...  ");
#endif
        g_ulTransferAddress = (uint32_t)APP_START_ADDRESS;
        ((void (*)(void))g_ulTransferAddress)();
    }

    //
	//  Configure the microcontroller.
	//
	//EnterBootLoader
	#ifdef CAN_ENABLE_UPDATE
	    ConfigureCANDevice(CAN_PORT);
	#endif
	#ifdef SPI_ENABLE_UPDATE
	    ConfigureSPIDevice(SPI_PORT);
	#endif
	#ifdef UART_ENABLE_UPDATE
	    ConfigureUartDevice();
	#endif

	//
	// Branch to the update handler. Use can1
	//
	#ifdef CAN_ENABLE_UPDATE
	    UpdaterCAN(CAN_PORT);
	#endif

	#ifdef UART_ENABLE_UPDATE
	    UpdaterUART();
	#endif
	#ifdef SPI_ENABLE_UPDATE
	    UpdaterSPI(SPI_PORT);
	#endif

}


/******************************************************************************
*
* Configures the microcontroller.
*
* This function configures the peripherals and GPIOs of the microcontroller,
* preparing it for use by the boot loader.  The interface that has been
* selected as the update port will be configured, and auto-baud will be
* performed if required.
*
* \return None.
*
******************************************************************************/
#ifdef SPI_ENABLE_UPDATE
void ConfigureSPIDevice(spiBASE_t *node)
{
    //
    // Initialize the SPI1 as slave mode
    // Enable the SPI interface in slave mode.
    // Set the SPI protocol to Motorola with default clock high and data valid on the rising edge.
    //
    spiInit();
}
#endif

#ifdef UART_ENABLE_UPDATE
void ConfigureUartDevice(void)
{
    //
    // Enable the the clocks to the UART and GPIO modules.
    //
   sciInit();
}
#endif

#endif

