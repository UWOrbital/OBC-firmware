//*****************************************************************************
//
// bl_ymodem.c    : The file holds the main control loop of the boot loader.
// Author         : QJ Wang. qjwang@ti.com
// Date           : 2-16-2016
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
/*  YMODEM MINIMUM REQUIREMENTS

    All programs claiming to support YMODEM must meet the following minimum
    requirements:
       + The sending program shall send the pathname (file name) in block 0.
       + The pathname shall be a null terminated ASCII string as described below.
            + Unless specifically requested, only the file name portion is sent.
            + No drive letter is sent.
            + Systems that do not distinguish between upper and lower case
              letters in filenames shall send the pathname in lower case only.

       + The receiving program shall use this pathname for the received file
         name, unless explicitly overridden.
       + When the receiving program receives this block and successfully
         opened the output file, it shall acknowledge this block with an ACK
         character and then proceed with a normal XMODEM file transfer
         beginning with a "C" or NAK tranmsitted by the receiver.
       + The sending program shall use CRC-16 in response to a "C" pathname
         nak, otherwise use 8 bit checksum.
       + The receiving program must accept any mixture of 128 and 1024 byte
         blocks within each file it receives.  Sending programs may
         arbitrarily switch between 1024 and 128 byte blocks.
       + The sending program must not change the length of an unacknowledged
         block.
       + At the end of each file, the sending program shall send EOT up to ten
         times until it receives an ACK character.  (This is part of the
         XMODEM spec.)
       + The end of a transfer session shall be signified by a null (empty)
         pathname, this pathname block shall be acknowledged the same as other
         pathname blocks.
*/

/*
   SENDER                                               RECEIVER
   ======                                               ========
                                                           C
                                                           C
   SOH 00 FF Filename NULs CRC CRC
                                                          ACK
                                                           C
   STX 01 FD Data[1024] CRC1 CRC2
                                                          ACK
   SOH 02 FC Data[128] CRC1 CRC2
                                                          ACK
   SOH 03 FB Data[100] ^Z[28] CRC1 CRC2
                                                          ACK
   EOT
                                                          NAK
   EOT
                                                          ACK
                                                           C
   SOH 00 FF NUL[128] CRC CRC
                                                          ACK

   Use a sequence of two consecutive CAN (Hex 18) characters as
   a transfer abort command.
 */

#include "bl_config.h"

#if defined(UART_ENABLE_UPDATE)
#include "sci_common.h"
#include "bl_flash.h"
#include "hw_sci.h"
#include "bl_ymodem.h"
#include "sys_common.h"
//#include "sys_pmu.h"

char fileName[FILENAME_LEN];
extern unsigned int g_pulUpdateSuccess[8];
extern unsigned int g_ulUpdateStatusAddr;
extern uint32_t g_ulUpdateBufferSize;         //32 bytes or 8 32-bit words
extern void delay(unsigned int delayval);


int Ymodem_Receive (sciBASE_t *sci, char *buf)
{
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_HEADER + PACKET_CRC]={NULL};
	unsigned char fileSize[FILESIZE_LEN], *pFile;
	char *pcBuf;
	int i, file_done, packets_received, errors, imageSize = 0, key;
	unsigned int oReturnCheck;
	unsigned char ucBank=0;
    unsigned short packet_size, loop=1;
    unsigned int FlashDestination = APP_START_ADDRESS ;            /* Flash user program offset */

	g_pulUpdateSuccess[1] = FlashDestination;
	enum rcvstates              // states when receiving
	{
	     AskHeader,             // ask for a header
	     WaitHeader,            // await header 1st char
	     EOTHeader,             // EOT process
	     EndSession,
	     CANHeader,
		 AbortHeader,
		 PacketErrors,
		 GetPacketData,
		 Packet0,
		 DataPacket
	} state;

	while(loop)
	{
		file_done = 0;
		packets_received = 0;
		pcBuf = buf;
		errors = 0;
		state = AskHeader;

		while(file_done == 0 )
		{
			switch (state)
			{
				/* The receiver starts by sending an “C” (0x43) to the sender indicating it wishes
				 * to use the CRC method of block validating. After sending the initial “C” the receiver waits
				 * for either a 3 second time out or until a buffer full flag is set. If the receiver is
				 * timed out then another “C” is sent to the sender and the 3 second time out starts again.
				 * This process continues until the receiver receives a complete 133-byte or 1029-byte packet.
				 */
				case AskHeader:
					 UART_txByte(sci, CRC);
					 state = WaitHeader;
					 break;
				case WaitHeader:
					 key = UART_getChar(sci, WAIT_TIMEOUT);
					 if (key < 0 ){
						 state = PacketErrors;  //timeout
						 break;
					 }

					 /* Byte 1 of the Ymodem packet can only have a value of SOH, STX, EOT, CAN
					  * anything else is an error
					  */
					 switch (key)
					 {
						 case SOH:
							  packet_size = PACKET_SIZE;
							  state = GetPacketData;
							  break;
						 case STX:
							  packet_size = PACKET_1K_SIZE;
							  state = GetPacketData;
							  break;
						 case EOT:
							  state = EOTHeader;
							  break;
						 case CAN:  //Cancel transfer.
							  key = UART_getChar(sci, WAIT_TIMEOUT);
							  if (key == CAN)  {
								  state = CANHeader;
							  }
						 default:  /* unexpected key, treat it as cancelled by sender */
							 state = PacketErrors;
							 break;
					 }
					 break;
				case EOTHeader:   /* End of transmission */
					 UART_txByte(sci, ACK);
					 file_done = 1;
					 break;
				case EndSession:
					 UART_txByte(sci, ACK);
					 file_done = 1;
					 loop = 0;  //sessioin done
					 break;
				case CANHeader:     /* Cancelled by sender */
					 UART_txByte(sci, ACK);  //for get 2 CAN from sender
					 return 0;
				case AbortHeader:  /* cancelled by receiver: errors, timeout, etc*/
					 UART_txByte(sci, CAN);
					 UART_txByte(sci, CAN);
					 return 0;
				case PacketErrors:  /* 1: CRC error; 2:Packet # != (it's complement ^ 0xFF) */
									/* 3: timeout */
					 if (packets_received != 0)
					 {
						 errors ++;
						 if(errors >= MAX_ALLOWED_ERRORS){
							state = AbortHeader;
							break;
						 }
					 }
					 UART_txByte(sci, CRC);
					 state = WaitHeader;
					 break;
				case GetPacketData:
					 *packet_data = (char) key;  //data[0] = SOH, STX

					 /* The protocol NAKs the following conditions:
					  * 1. Framing error on any byte : serious hardware failures, baudrate etc
					  * 2. Overrun error on any byte : serious hardware failures, baudrate etc
					  * 3. Duplicate packet: the sender getting an ACK garbled and re-transmitting the packet.
					  * 4. CRC error : noisy environments
					  * 5. Receiver timed out (didn't receive packet within x second)
					  * On any NAK, the sender will re-transmit the last packet.
					  */
					 for (i = 1; i < (packet_size +  PACKET_HEADER + PACKET_CRC); i++)
					 {
						 key = UART_getChar(sci, WAIT_TIMEOUT);
						 if(key < 0) {
							 state = PacketErrors;
							 break;
						  }
						 packet_data[i] = (char) key;
					 }
					 if ( (packet_data[PACKET_SEQNUM_LOC] + packet_data[PACKET_SEQNUM_LOC+1]) != 255 )
					 {
						 state = PacketErrors;
						 break;
					 }
					 if (_cal_crc16_(packet_data + PACKET_HEADER, packet_size + PACKET_CRC) != 0) {
						 state = PacketErrors;
						 break;
					 }
					 /* Duplicate packet */
					 if ((packet_data[PACKET_SEQNUM_LOC] & 0xff) != (packets_received & 0xff)) {
						 UART_txByte(sci, NAK);
						 state = WaitHeader;
					 }
					 else
					 {
						if (packets_received == 0)
						{
							state = Packet0;
						}else{
							state = DataPacket;
						}
						packets_received ++;
					 }
					 break;

				case Packet0:
					 /* Filename packet:
					  * If directories are included, they are delimited by / rather than \
					  * The filename is sent as a NUL terminated ASCII string
					  * No spaces are included in the pathname or filename
					  */
					 if (packet_data[PACKET_HEADER] != 0)  /* The filename is not NULL*/
					 {
						/* Filename packet has valid data */
						for (i = 0, pFile = packet_data + PACKET_HEADER; (*pFile != 0) && (i < FILENAME_LEN);)
						{
							fileName[i++] = *pFile++;
						}
						fileName[i++] = '\0';
						for (i = 0, pFile ++; (*pFile != ' ') && (i < FILESIZE_LEN);)
						{
						  fileSize[i++] = *pFile++;
						}
						fileSize[i++] = '\0';
						Str2Int(fileSize, &imageSize);
						g_pulUpdateSuccess[2] = (uint32_t) imageSize;
						/* Test the size of the image to be sent */
						/* Image size is greater than Flash size */
						/* Erase the FLASH pages */
						if(!BLInternalFlashStartAddrCheck(FlashDestination,  imageSize))
						{
							state = AbortHeader;
						}
						/* Initialize the Flash Wrapper registers */
						oReturnCheck = 0;
						oReturnCheck = Fapi_BlockErase( ucBank, FlashDestination, imageSize);
						// Return an error if an access violation occurred.
						if(oReturnCheck)
						{
							state = AbortHeader;
						}
						//Tell sender we get the header successfully
						UART_txByte(sci, ACK);
						delay(5000000);     //0.5s
						UART_txByte(sci, CRC);
						state = WaitHeader;
					 }
					 /* packet_data[PACKET_HEADER] = 0: Filename packet is empty, end session */
					 else {
							state = EndSession;
					 }  //end of "if (packet_data[PACKET_HEADER] != 0), else"
					 break;

				case DataPacket:
					 oReturnCheck = 0;
					 memcpy(pcBuf, packet_data + PACKET_HEADER, packet_size);
					 oReturnCheck = Fapi_BlockProgram( ucBank, FlashDestination, (unsigned long)buf, packet_size);
					 if(oReturnCheck)
					 {
						// Indicate that the flash programming failed.
						state = AbortHeader;
						break;
					 }
					 // Now update the address to program.
					 FlashDestination += packet_size;
					 UART_txByte(sci, ACK);
					 state = WaitHeader;
					 break;
			}
		}//while(file_done)
	}//while(loop)


	if( errors == 0)
    {
    	oReturnCheck = Fapi_BlockProgram( ucBank, g_ulUpdateStatusAddr, (unsigned long)&g_pulUpdateSuccess, g_ulUpdateBufferSize);
    }
	return (int)imageSize;
}


/**
  * Description: Transmit a single packet using the ymodem protocol
  * Parameters:  sci port, data, length
  * Return value:  None
  */
static void SendPacket(sciBASE_t *sci, unsigned char *data, unsigned int packetNum)
{
	  unsigned short i = 0;
	  int crc16, packet_size;
	  char ch;

	  if (packetNum==0){
		  packet_size = PACKET_SIZE;
	  }else{
		  packet_size = PACKET_1K_SIZE;
	  }

	  crc16 = _cal_crc16_(data, packet_size);

	  if (packetNum == 0)
		  UART_txByte(sci, SOH);
	  else
		  UART_txByte(sci, STX);

	  ch = packetNum & 0xFF;
	  UART_txByte(sci, ch);
	  UART_txByte(sci, 0xFF-ch);

	  for(i=0; i < packet_size; i++)
	  {
		  UART_txByte(sci, data[i]);
	  }
	  UART_txByte(sci, (crc16 >> 8) & 0xFF);
	  UART_txByte(sci, crc16 & 0xFF);
}

void SendDataPacket(sciBASE_t *sci, unsigned char *data, unsigned int size)
{
	  unsigned int send_size, i;
	  int blockno =1;
	  int key, ch;
	  unsigned char temp[PACKET_SIZE];

	  while (size > 0)
	  {
		  if (size > PACKET_1K_SIZE)
			  send_size = PACKET_1K_SIZE;
		  else
			  send_size = size;

          //try upto 5 times
		  do
		  {
			  SendPacket(sci, data, blockno);

			  key = UART_getChar(sci, WAIT_TIMEOUT);

			  switch (key)
			  {
				 case ACK:
					  blockno++;
					  data += send_size;
					  size -= send_size;
					  i=5;
					  break;
				 case CRC:
					  i++;
			          break;
				 case NAK:
					  i++;
					  break;
				 case CAN:
					  return;
				 default:
					  return;
			  }//switch case
		  }while(i<5);
	  }//while loop

	  i=0;
      while (i < PACKET_SIZE) {
              temp[i++] = 0;
      }

	  //Wait for receiver's ACKnowledge
      //And send a null packet
	  while(1)
	  {
		  UART_txByte(sci,EOT);
		  key = UART_getChar(sci, WAIT_TIMEOUT);
		  switch(key)
		  {
		  	  case ACK:
		      	  ch = UART_getChar(sci, WAIT_TIMEOUT);
		      	  if (ch == CRC){
		      		  do{
		      			  SendPacket(sci, temp, 0);
		      			  ch = UART_getChar(sci, WAIT_TIMEOUT);
		      		  }while((ch !=ACK) && (ch != -1));
		          }
		      	  return;
		  	  case NAK:
		  		  continue;
		  	  case -1:
		  		  continue;
		  	  default:
		  		  return;
		  }
	  }
}//funtion end


/********************************************************************
   SENDER                                               RECEIVER
   ======                                               ========
                                                           C
                                                           C
   SOH 00 FF Filename NULs CRC CRC
                                                          ACK
                                                           C
   STX 01 FD Data[1024] CRC1 CRC2
                                                          ACK
   SOH 02 FC Data[128] CRC1 CRC2
                                                          ACK
   SOH 03 FB Data[100] ^Z[28] CRC1 CRC2
                                                          ACK
   EOT
                                                          NAK
   EOT
                                                          ACK
                                                           C
   SOH 00 FF NUL[128] CRC CRC
                                                          ACK
 ********************************************************************/

char Ymodem_Transmit (sciBASE_t *sci, unsigned char *pImage, char* fileName, unsigned int imageSize)
{
	  int key, c;
	  unsigned int isFirst = 1;
      unsigned long i = 0;
      unsigned char packet_data[PACKET_SIZE];
      const char* sizeStr;
	  unsigned int blocknum = 0;

	  for(;;)
	  {
			/* Send file name Packet */
		    blocknum = 0;  //first block for file name and file size, 128 bytes long
		    if (fileName)
		    {    /*Packet0 like this: SOH 00 FF filename NULL filesize 00 00 00 ...*/
		         while (*fileName && (i < FILENAME_LEN))
		         {
		              packet_data[i++] = *fileName++;
		         }
		         packet_data[i++] = 0;
		         sizeStr = U32toStr(imageSize);
		         while(*sizeStr) {
		                packet_data[i++] = *sizeStr++;
		         }
		    }
 	        while (i < PACKET_SIZE) {
 	        	packet_data[i++] = 0;
		    }
		    /* Send the file packet which contain filename and file size */
		    /* The packet number is 0 */
		    SendPacket(sci, packet_data, blocknum);

    		/* Wait for ACK and 'C' after send file packet
    		 * Wait for ACK only for sending data packet
    		 */
			key = UART_getChar(sci, WAIT_TIMEOUT);
			if (key == ACK)
			{
				 c = UART_getChar(sci, WAIT_TIMEOUT);
				 if (c == CRC)
				 {
					 SendDataPacket(sci, pImage, imageSize);
					 return 0;
				 }
			}//end of if (key == ACK)
			else if ((key == CRC) && (isFirst == 1))
			{
					isFirst = 0;
					continue;       //jump back to for(;;)
			}
			else if (( key != NAK) || (isFirst == 1))
			{
					break;   //terminate the for(;;) loop
			}
	  }

	  UART_txByte(sci, CAN);
	  UART_txByte(sci, CAN);
	  return 0;
}

#endif

