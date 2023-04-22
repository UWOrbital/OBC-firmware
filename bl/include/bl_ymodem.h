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

#ifndef _YMODEM_H_
#define _YMODEM_H_


#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_sci.h"
#include "HL_sys_common.h"
#else
#include "sci.h"
#include "sys_common.h"
#endif

#define PACKET_SEQNUM_LOC       1    /* The byte 1, the byte 2 is its complement*/

#define PACKET_HEADER           3    /* SOH 00 FF; or STX 01 FE, etc */
#define PACKET_CRC              2    /* 2 bytes CRC */
#define PACKET_SIZE             128  /* The packet in SOH, normally for 1st packet used for filename and file szie*/
#define PACKET_1K_SIZE          1024 /* For packet in STX */

#define FILENAME_LEN            96   /* The filename cannot include space */
#define FILESIZE_LEN            16   /* I use unsigned int which is 4 bytes long */

#define SOH                     0x01  /* start of 128-byte data packet */
#define STX                     0x02  /* start of 1024-byte data packet */
#define EOT                     0x04  /* end of transmission */
#define ACK                     0x06  /* acknowledge */
#define NAK                     0x15  /* negative acknowledge */
#define CAN                     0x18  /* two of these in succession aborts transfer */
#define CRC                     0x43  /* 'C' == 0x43, request 16-bit CRC */

/* 0x500000 takes 178258017 CPU cycles, 1.114 seconds for 160Mhz CPU*/
/* 0x480000 takes 160432221 CPU cycles, 1.003 seconds for 160Mhz CPU*/
#if defined (TMS570LS04) || defined (RM42)   //LS04 and RM42 runs 80MHz
#define WAIT_TIMEOUT            0x240000   //Timeout=1.003 seconds
#else
#define WAIT_TIMEOUT            0x100000   //Timeout=1.003 seconds
#endif

#define MAX_ALLOWED_ERRORS            100

char Ymodem_Transmit (sciBASE_t *sci, uint8_t *buf, char* fileName, uint32_t fileSize);
int  Ymodem_Receive (sciBASE_t *sci, char *buf);
int _cal_crc16_(unsigned char*, int);

#endif  /* _YMODEM_H_ */

