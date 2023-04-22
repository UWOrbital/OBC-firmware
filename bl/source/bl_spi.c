//*****************************************************************************
//
// bl_spi.c - Functions used to transfer data via the SPI port.
// Author         : QJ Wang. qjwang@ti.com
// Date           : 9-19-2012
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


#if defined(SPI_ENABLE_UPDATE)

#include "bl_commands.h"
#include "bl_spi_packet.h"
#include "bl_spi.h"
#include "Registers_FMC_LE.h"
#include "system.h"
#include "bl_flash.h"
#include "hw_spi.h"
#include "sci_common.h"
#include "bl_led_demo.h"

//*****************************************************************************

//*****************************************************************************
//
// Holds the current status of the last command that was issued to the boot
// loader.
//
//*****************************************************************************
uint8_t g_ucStatus;

//*****************************************************************************
//
// Holds the current address to write to when data is received via the Send
// Data Command.
//
//*****************************************************************************
extern uint32_t g_ulTransferAddress;

//*****************************************************************************
//
// Holds the remaining bytes expected to be received.
//
//*****************************************************************************
extern uint32_t g_ulTransferSize;


//*****************************************************************************
//
// These globals are used to store the first two words to prevent a partial
// image from being booted.
//
//*****************************************************************************
extern uint8_t *g_pucDataBuffer;
extern uint32_t g_pulDataBuffer[BUFFER_SIZE];
extern uint32_t g_pulUpdateSuccess[8];
extern uint32_t g_ulUpdateStatusAddr;
extern uint32_t g_ulUpdateBufferSize;  //32 bytes or 8 32-bit words

//! Sends data via the SPI port in slave mode.
//!
//! \param pucData is the location of the data to send through the SPI port.
//! \param ulSize is the number of bytes of data to send.
//!
//! This function sends data through the SPI port in slave mode.  This function
//! will not return until all bytes are sent.
//!
//! \return None.
//
//*****************************************************************************
void
SPISend(spiBASE_t *node, const uint8_t *pucData, uint8_t ulSize)
{
	spiDAT1_t dataconfig_t;
    volatile uint32_t SpiBuf;
    uint32_t errors;

	dataconfig_t.CS_HOLD = 1;
	dataconfig_t.CSNR = 1;
	dataconfig_t.DFSEL = 0;
	dataconfig_t.WDEL = 0;

    errors = spiTransmitData(node, &dataconfig_t, (uint16_t)ulSize, (uint8_t *)pucData);
}


//*****************************************************************************
//
//! Receives data from the SPI port in slave mode.
//!
//! \param pucData is the location to store the data received from the SPI
//! port.
//! \param ulSize is the number of bytes of data to receive.
//!
//! This function receives data from the SPI port in slave mode.  The function
//! will not return until \e ulSize number of bytes have been received.
//!
//! \return None.
//
//*****************************************************************************
void
SPIReceive(spiBASE_t *node, uint8_t *pucData, uint8_t ulSize)
{
	spiDAT1_t dataconfig_t;
    uint32_t errors;
	
	dataconfig_t.CS_HOLD = 1;
	dataconfig_t.CSNR = 1;
	dataconfig_t.DFSEL = 0;
	dataconfig_t.WDEL = 0;

   errors = spiReceiveData(node, &dataconfig_t, (uint16_t)ulSize, (uint8_t*)pucData);

}


void UpdaterSPI(spiBASE_t *node)
{
    uint32_t ulFlashSize;
    uint32_t oReturnCheck;
	uint32_t FLASH_FIRST_SECTOR_SIZE;
	uint8_t  ucBank, ulSize;

	uint32_t  updateStatusBank;
	uint8_t ucUpdateStatus;

    updateStatusBank = 0;
    ucUpdateStatus = 0;

    ucBank = 0;
	FLASH_FIRST_SECTOR_SIZE = BLInternalFlashFirstSectorSizeGet();

    // This ensures proper alignment of the global buffer so that the one byte
    // size parameter used by the packetized format is easily skipped for data
    // transfers.
    // 1st byte is size, 2nd byte is checksum, the 3rd byte is COMMAND, the 4th is DATA
    g_pucDataBuffer = ((uint8_t *)g_pulDataBuffer) + 3;

    // Insure that the SPI_COMMAND_SEND_DATA cannot be sent to erase the boot
    // loader before the application is erased.
    g_ulTransferAddress = 0xffffffff;

    // Read any data from the serial port in use.

    while(1)
    {
        // Receive a packet from the port in use.
        // ReceivePacket(): Returns zero to indicate success while any non-zero value indicates a failure.
    	LITE_TOPRIGHT_LED ;

        ulSize = sizeof(g_pulDataBuffer) - 3;

#if 1
        //the ulSize returned from ReceivPacket is the packet size excluding the size( and the checksum(2nd byte)
        if(ReceivePacket(node, g_pucDataBuffer, &ulSize) != 0)
        {
            continue;   //skip and move back to the while() loop
        }
#endif
        // The first byte of the data buffer has the command and determines
        // the format of the rest of the bytes.
        switch(g_pucDataBuffer[0])
        {
            // #1, This was a simple ping command.
            case SPI_COMMAND_PING:  /*0x20*/
            {
                // This command always sets the status to SPI_COMMAND_RET_SUCCESS.
                g_ucStatus = SPI_COMMAND_RET_SUCCESS;
                AckPacket(node);
                // Just acknowledge that the command was received.
                // Go back and wait for a new command.
                break;
            }

            //
            // #2, This command indicates the start of a download sequence.
            //
            case SPI_COMMAND_DOWNLOAD:  /*0x21*/
            {
                // g_pucDataBuffer[0] = 0x24;
            	LITE_BOTLEFT_LED ;

                // Until determined otherwise, the command status is success.
                g_ucStatus = SPI_COMMAND_RET_SUCCESS;

                // A simple do/while(0) control loop to make error exits easier.
                do
                {
                    // See if a full packet was received.
                    if(ulSize != 9)
                    {
                        // Indicate that an invalid command was received.
                        g_ucStatus = SPI_COMMAND_RET_INVALID_CMD;

                        // This packet has been handled.
                        break;
                    }

                    // Get the address and size from the command.
                    // where to swap the bytes?
                    // The data is transferred most significant bit (MSB) first
                    //This is used for RM48 which is little endian device
                    g_ulTransferAddress = 0|(g_pucDataBuffer[1]<<24)|(g_pucDataBuffer[2]<<16)|(g_pucDataBuffer[3]<<8)|(g_pucDataBuffer[4]<<0);

                    //Tell bootloader how many bytes the host will transfer for the whole application
                    g_ulTransferSize = 0|(g_pucDataBuffer[5]<<24)|(g_pucDataBuffer[6]<<16)|(g_pucDataBuffer[7]<<8)|(g_pucDataBuffer[8]<<0);

                    g_pulUpdateSuccess[1]= g_ulTransferAddress;
                    g_pulUpdateSuccess[2]= g_ulTransferSize;

                    // Check for a valid starting address and image size.
                    if(!BLInternalFlashStartAddrCheck(g_ulTransferAddress,  g_ulTransferSize))
                    {
                        // Set the code to an error to indicate that the last
                        // command failed.  This informs the updater program
                        // that the download command failed.
                        g_ucStatus = SPI_COMMAND_RET_INVALID_ADR;

                        // This packet has been handled.
                        break;
                    }

                    // Only erase the space that we need if we are not
                    // protecting the code, otherwise erase the entire flash.
                    ulFlashSize = g_ulTransferAddress + g_ulTransferSize;

                    /* Initialize the Flash Wrapper registers */
                    oReturnCheck = 0;
                    oReturnCheck = Fapi_BlockErase( ucBank, g_ulTransferAddress, g_ulTransferSize);

                    // Return an error if an access violation occurred.
                    //
                    if(oReturnCheck)
                    {
                    	g_ucStatus = SPI_COMMAND_RET_FLASH_FAIL;
                    }
                }
                while(0);

                //
                // See if the command was successful.
                //
                if(g_ucStatus != SPI_COMMAND_RET_SUCCESS)
                {
                    //
                    // Setting g_ulTransferSize to zero makes SPI_COMMAND_SEND_DATA
                    // fail to accept any data.
                    //
                    g_ulTransferSize = 0;
                }

                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //

                AckPacket(node);

                //
                // Go back and wait for a new command.
                //
                break;
            }

            // #3, This command indicates that control should be transferred to
            // the specified address.

            case SPI_COMMAND_RUN: /*0x22*/
            {
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.

                AckPacket(node);

                // See if a full packet was received.
                if(ulSize != 5)
                {
                    // Indicate that an invalid command was received.
                    g_ucStatus = SPI_COMMAND_RET_INVALID_CMD;

                    // This packet has been handled.
                    break;
                }

                // Get the address to which control should be transferred.
                g_ulTransferAddress = 0|(g_pucDataBuffer[1]<<24)|(g_pucDataBuffer[2]<<16)|(g_pucDataBuffer[3]<<8)|(g_pucDataBuffer[4]<<0);

                // This determines the size of the flash available on the
                // device in use.
                ulFlashSize = 0x300000;  //3MB, need to check the register to get the size automatically

                // Test if the transfer address is valid for this device.
                if(g_ulTransferAddress >= ulFlashSize)
                {
                    // Indicate that an invalid address was specified.
                    g_ucStatus = SPI_COMMAND_RET_INVALID_ADR;

                    // This packet has been handled.
                    break;
                }

                // Make sure that the ACK packet has been sent.

                // Reset and disable the peripherals used by the boot loader.
                node->GCR0 = 0U;
                node->GCR0 = 1U;
                node->GCR1 &= ~(0x1 << 24);   //De-activate SPI for transmission

                // Branch to the specified address.  This should never return.
                // If it does, very bad things will likely happen since it is
                // likely that the copy of the boot loader in SRAM will have
                // been overwritten.

                ((void (*)(void))g_ulTransferAddress)();

                // In case this ever does return and the boot loader is still
                // intact, simply reset the device.
                // Use the reset in SYSECR register.
                systemREG1->SYSECR = (0x10) << 14;

                // The microcontroller should have reset, so this should
                // never be reached.  Just in case, loop forever.
                while(1)
                {
                }
            }

            // #4, This command just returns the status of the last command that was sent.
            case SPI_COMMAND_GET_STATUS:    /*0x23*/
            {
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
            	AckPacket(node);

            	SendPacket(node, &g_ucStatus, 1);

            	// Go back and wait for a new command.
                break;
            }

            // #5, This command is sent to transfer data to the device following a download command.
            case SPI_COMMAND_SEND_DATA:  /* 0x24*/
            {
                // Until determined otherwise, the command status is success.
            	LITE_BOTRIGHT_LED ;

                g_ucStatus = SPI_COMMAND_RET_SUCCESS;

                // If this is overwriting the boot loader then the application
                // has already been erased so now erase the boot loader.
                //for over-writing the bootloader
	            if(g_ulTransferAddress == 0)
                {
                    // Erase the boot loader.
						/* Initialize the Flash Wrapper registers */
						oReturnCheck = 0;

						oReturnCheck = Fapi_BlockErase( 0, 0, FLASH_FIRST_SECTOR_SIZE);

						// Return an error if an access violation occurred.
						if(oReturnCheck)
						{
							// Setting g_ulTransferSize to zero makes
							// SPI_COMMAND_SEND_DATA fail to accept any more data.
							g_ulTransferSize = 0;

							// Indicate that the flash erase failed.
							g_ucStatus = SPI_COMMAND_RET_FLASH_FAIL;
						}
                } //end of erasing the boat loader section
	            //finish bootloader over-writing

                // Take one byte off for the command.
                ulSize = ulSize - 1;

                // Check if there are any more bytes to receive.
                if(g_ulTransferSize >= ulSize)
                {
						  /* Initialize the Flash Wrapper registers */
						oReturnCheck = 0;

						oReturnCheck = Fapi_BlockProgram( ucBank, g_ulTransferAddress, (uint32_t)&g_pulDataBuffer[1], ulSize);

						// Return an error if an access violation occurred.
						if(oReturnCheck)
						{
							// Indicate that the flash programming failed.
							g_ucStatus = SPI_COMMAND_RET_FLASH_FAIL;
							ucUpdateStatus = 0;
							UART_putString(UART, "\r Program Flash failed:  ");
						}
						else
						{
							// Now update the address to program.
							g_ulTransferSize -= ulSize;
							g_ulTransferAddress += ulSize;
							ucUpdateStatus = 1;
						}
                }
                else
                {
						// This indicates that too much data is being sent to the device.
						g_ucStatus = SPI_COMMAND_RET_INVALID_ADR;
						ucUpdateStatus = 0;
                }

                if (g_ulTransferSize == 0)
                {
                	if( ucUpdateStatus == 1)
                	{
                		oReturnCheck = Fapi_UpdateStatusProgram( updateStatusBank, g_ulUpdateStatusAddr, (uint32_t)&g_pulUpdateSuccess[0], g_ulUpdateBufferSize);
                 		UART_putString(UART, "\r Application was loaded successful!  ");
                	}
            	}

                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was received.
                AckPacket(node);

               // Go back and wait for a new command.
                break;
            }

            //
            // #6, This command is used to reset the device.
            //
            case SPI_COMMAND_RESET:  /*0x25*/
            {
                //
                // Send out a one-byte ACK to ensure the byte goes back to the
                // host before we reset everything.
                //
                AckPacket(node);
                //
                // Make sure that the ACK packet has been sent.
                //

                //
                // Perform a software reset request.  This will cause the
                // microcontroller to reset; no further code will be executed.
                //
                // Use the reset in SYSECR register.
                //
                systemREG1->SYSECR = (0x10) << 14;

                //
                // The microcontroller should have reset, so this should never
                // be reached.  Just in case, loop forever.
                //
                while(1)
                {
                }
            }

            //
            // Just acknowledge the command and set the error to indicate that
            // a bad command was sent.
            //
            default:
            {
                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //
                //AckPacket();

                //
                // Indicate that a bad comand was sent.
                //
                g_ucStatus = SPI_COMMAND_RET_UNKNOWN_CMD;

                //
                // Go back and wait for a new command.
                //
                break;
            }
        }
    }
}

#endif
