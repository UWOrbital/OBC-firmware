//*****************************************************************************
//
// bl_packet.c    : Packet handler functions used by the boot loader.
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
#include "bl_commands.h"
#include "bl_spi_packet.h"
#include "bl_spi.h"
#include "bl_uart.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_sci.h"
#else
#include "sci.h"
#endif


#if defined (SPI_ENABLE_UPDATE)

//*****************************************************************************
//
// The packet that is sent to acknowledge a received packet.
//
//*****************************************************************************
static const uint8_t g_pucACK[2] = { 0, SPI_COMMAND_ACK };

//*****************************************************************************
//
// The packet that is sent to not-acknowledge a received packet.
//
//*****************************************************************************
static const uint8_t g_pucNAK[2] = { 0, SPI_COMMAND_NAK };

//*****************************************************************************
//
//! Calculates an 8-bit checksum
//!
//! \param pucData is a pointer to an array of 8-bit data of size ulSize.
//! \param ulSize is the size of the array that will run through the checksum
//! algorithm.
//!
//! This function simply calculates an 8-bit checksum on the data passed in.
//!
//! \return Returns the calculated checksum.
//
//*****************************************************************************
uint32_t
CheckSum(const uint8_t *pucData, uint32_t ulSize)
{
    uint32_t ulCheckSum;

    //
    // Initialize the checksum to zero.
    //
    ulCheckSum = 0;

    //
    // Add up all the bytes, do not do anything for an overflow.
    //
    while(ulSize--)
    {
        ulCheckSum += *pucData++;
    }

    //
    // Return the calculated check sum.
    //
    return(ulCheckSum & 0xff);
}

//*****************************************************************************
//
//! Sends an Acknowledge packet.
//!
//! This function is called to acknowledge that a packet has been received by
//! the microcontroller.
//!
//! \return None.
//
//*****************************************************************************
void
AckPacket(spiBASE_t *node)
{
    //
    // ACK/NAK packets are the only ones with no size.
    //
    SendData(node, g_pucACK, 2);
}

//*****************************************************************************
//
//! Sends a no-acknowledge packet.
//!
//! This function is called when an invalid packet has been received by the
//! microcontroller, indicating that it should be retransmitted.
//!
//! \return None.
//
//*****************************************************************************
void
NakPacket(spiBASE_t *node)
{
    //
    // ACK/NAK packets are the only ones with no size.
    //
    SendData(node, g_pucNAK, 2);
}

//*****************************************************************************
//
//! Receives a data packet.
//!  The 1st byte is the packet size, 2nd byte is the checksum, the 3rd & later are data
//!   
//! \param pucData is the location to store the data that is sent to the boot loader.
//! \param pulSize is the number of bytes returned in the pucData buffer that was provided.
//!
//! This function receives a packet of data from specified transfer function.
//!
//! \return Returns zero to indicate success while any non-zero value indicates a failure.
//
//*****************************************************************************
int
ReceivePacket(spiBASE_t *node, uint8_t *pucData, uint8_t *pulSize)
{
    uint8_t ulSize, ulCheckSum;

    //
    // Wait for non-zero data before getting the first byte that holds the
    // size of the packet we are receiving.
    //
    ulSize = 0;
    while(ulSize == 0)
    {
        ReceiveData(node, (uint8_t *)&ulSize, 1);
    }

    //
    // Subtract off the size and checksum bytes.
    //
    ulSize -= 2;

    //
    // Receive the checksum followed by the actual data.
    //
    ReceiveData(node, (uint8_t *)&ulCheckSum, 1);

    //
    // If there is room in the buffer then receive the requested data.
    //
    if(*pulSize >= ulSize)
    {
        //
        // Receive the actual data in the packet.
        //
        ReceiveData(node, pucData, ulSize);

#if 0
        // Send a no acknowledge if the checksum does not match, otherwise send
        // an acknowledge to the packet later.
        if(CheckSum(pucData, ulSize) != (ulCheckSum & 0xff))
        {
            // Indicate tha the packet was not received correctly.
            NakPacket(node);

            // Packet was not received, there is no valid data in the buffer.
            return(-1);
        }
#endif

    }
    else
    {
        //
        // If the caller allocated a buffer that was too small for the received
        // data packet, receive it but don't fill the buffer.
        // Then inform the caller that the packet was not received correctly.
        //
        while(ulSize--)
        {
            ReceiveData(node, pucData, 1);
        }

        //
        // Packet was not received, there is no valid data in the buffer.
        //
        return(-1);
    }

    //
    // Make sure to return the number of bytes received.
    //
    *pulSize = ulSize;

    //
    // Packet was received successfully.
    //
    return(0);
}

//*****************************************************************************
//
//! Sends a data packet.
//!
//! \param pucData is the location of the data to be sent.
//! \param ulSize is the number of bytes to send.
//!
//! This function sends the data provided in the \e pucData parameter in the
//! packet format used by the boot loader.  The caller only needs to specify
//! the buffer with the data that needs to be transferred.  This function
//! addresses all other packet formatting issues.
//!
//! \return Returns zero to indicate success while any non-zero value indicates
//! a failure.
//
//*****************************************************************************
int
SendPacket(spiBASE_t *node, uint8_t *pucData, uint8_t ulSize)
{
    uint32_t ulTemp;

    //
    // Calculate the checksum to be sent out with the data.
    //
    ulTemp = CheckSum(pucData, ulSize);

    //
    // Need to include the size and checksum bytes in the packet.
    //
    ulSize += 2;

    //
    // Send out the size followed by the data.
    //
    SendData(node, (uint8_t *)&ulSize, 1);
    SendData(node, (uint8_t *)&ulTemp, 1);
    SendData(node, pucData, ulSize - 2);

    //
    // Wait for a non zero byte.
    //
    ulTemp = 0;
    while(ulTemp == 0)
    {
        ReceiveData(node, (uint8_t *)&ulTemp, 1);
    }

    //
    // Check if the byte was a valid ACK and return a negative value if it was
    // not and acknowledge.
    //
    if(ulTemp != SPI_COMMAND_ACK)
    {
        return(-1);
    }

    //
    // This packet was sent and received successfully.
    //
    return(0);
}

#endif
