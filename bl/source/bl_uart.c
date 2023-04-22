//*****************************************************************************
//
// bl_uart.c : Functions to transfer data via the UART port.
// Author    : QJ Wang. qjwang@ti.com
// Date      : 9-19-2012
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
#include "bl_config.h"

#if defined(UART_ENABLE_UPDATE)
#include "hw_sci.h"
#include "bl_uart.h"
#include "sci_common.h"
#include "bl_ymodem.h"
#include "sys_common.h"
#include "bl_led_demo.h"

uint32_t JumpAddress;
void get_software_Version(void);
void get_hardware_Info(void);

extern uint32_t g_pulUpdateSuccess[8];
extern uint32_t g_ulUpdateStatusAddr;
extern uint32_t g_ulUpdateBufferSize;  //32 bytes or 8 32-bit words
extern char fileName[FILENAME_LEN];
char tab_1024[1024] = {    0  };

//*****************************************************************************
//
//  bl_uart_api
//
//
//*****************************************************************************
/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void UART_Download()
{
  char Number[10] = "          ";
  int Size = 0;

  UART_putString(UART, "\n\r Waiting for the file to be sent ... ");
  UART_putString(UART, "\n\r Use Transfer->Send File and Ymodem Protocol from HyperTerminal \n\r");

  Size = Ymodem_Receive(UART, &tab_1024[0]);
  if (Size > 0)
  {
    UART_putString(UART, "\n\r The application image has been programmed successfully!\n\r---------------------------\n\r Name: ");
    UART_putString(UART, fileName);
    Int2Str(Number, Size);
    UART_putString(UART, "\n\n\r Size:     ");
    UART_putString(UART, Number);
    UART_putString(UART, "  Bytes\n\r");
    UART_putString(UART, "---------------------------\n\n\n\n\r");
  }
  else
  {
    UART_putString(UART, "\n\rFailed to receive the file!\n\r");
  }
}

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void UART_Upload(void)
{
	  uint32_t status = 0;
	  uint32_t *updateInfo;
	  uint32_t imageSize;
	  uint32_t imageAddress;

	  updateInfo = (uint32_t *)g_ulUpdateStatusAddr;

	  imageAddress = updateInfo[1];
	  imageSize = updateInfo[2];


	  UART_putString(UART, "\n\n\rSelect Receive File and Ymodel protocol in the drop-down menu... (press any key to abort)\n\r");

	  if (UART_getKey(UART) == CRC )
	  {
			/* Transmit the flash image through ymodem protocol */
			status = Ymodem_Transmit(UART, (uint8_t*)imageAddress, (char*)"UploadedApplicationImage.bin", imageSize);

			if (status != 0)
			{
				UART_putString(UART, "\n\rError occured while transmitting\n\r");
			}
			else
			{
				UART_putString(UART, "\n\rApplication image has been transmitted successfully \n\n\n\r");
			}
	  }
	  else
	  {
		  UART_putString(UART, "\r\n\nAborted by user.\n\r");
	  }

}

//*****************************************************************************
//
//! This function performs the update on the selected port.
//!
//! This function is called directly by the boot loader or it is called as a
//! result of an update request from the application.
//!
//! \return Never returns.
//
//*****************************************************************************
void UpdaterUART(void)
{
	  char key = 0;

	  while (1)
	  {
		    LITE_TOPRIGHT_LED;
			UART_putString(UART, "\r================== Main Menu ==========================\r\n");
			UART_putString(UART, "  1. Download Application Image To the Internal Flash \r\n");
			UART_putString(UART, "  2. Upload The Application Image From the Internal Flash \r\n");
			UART_putString(UART, "  3. Execute The Application Code \r\n");
			UART_putString(UART, "  4. Get Bootloader Version \r\n");
			UART_putString(UART, "  5. Get Device Information \r\n");

			UART_putString(UART,   "=======================================================\r\n\n");

			key = UART_getKey(UART);

			if (key == 0x31)
			{
			   /* Download user application in the Flash */
			   UART_Download();
			}
		    else if (key == 0x32)
		    {
		      /* Upload user application from the Flash */
		      UART_Upload();
		    }
			else if (key == 0x33)
			{
				UART_putString(UART, "The application is running now !! \n\r\n\r");
		        JumpAddress = (uint32_t)APP_START_ADDRESS;
		        ((void (*)(void))JumpAddress)();
			}
			else if (key == 0x34)
			{
				get_software_Version();
			}
			else if (key == 0x35)
			{
				get_hardware_Info();
			}
			else
			{
				UART_putString(UART, "Invalid Number !! \n\r\n\r");
			}
	  }
}

#endif


