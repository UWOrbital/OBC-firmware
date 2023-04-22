//*****************************************************************************
//
// bl_can.h - Definitions for the CAN transport functions.
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

#ifndef __BL_CAN_H__
#define __BL_CAN_H__


#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_can.h"
#include "HL_sys_common.h"
#else
#include "can.h"
#include "sys_common.h"
#endif

//*****************************************************************************
//
// The following are defines for the bit fields in the CAN_IFCMD register.
//
//*****************************************************************************
#define CAN_IFCMD_BUSY         0x80  // Busy Flag
#define CAN_IFCMD_MNUM_M       0xFF  // Message Number
#define CAN_IFCMD_MNUM_RSVD    0x00000000  // 0 is not a valid message number;
#define CAN_IFCMD_MNUM_S       0

#define CAN_IFCMD_WRNRD       0x80 // Write, Not Read
#define CAN_IFCMD_MASK        0x40  // Access Mask Bits
#define CAN_IFCMD_ARB         0x20  // Access Arbitration Bits
#define CAN_IFCMD_CONTROL     0x10  // Access Control Bits
#define CAN_IFCMD_CLRINTPND   0x08  // Clear Interrupt Pending Bit
#define CAN_IFCMD_NEWDAT      0x04  // Access New Data
#define CAN_IFCMD_TXRQST      0x04  // Access Transmission Request
#define CAN_IFCMD_DATAA       0x02  // Access Data Byte 0 to 3
#define CAN_IFCMD_DATAB       0x01  // Access Data Byte 4 to 7

//*****************************************************************************
//
// The following are defines for the bit fields in the IF1MSK register.
//
//*****************************************************************************
#define CAN_IFMSK_MXTD         0x80000000  // Mask Extended Identifier
#define CAN_IFMSK_MDIR         0x40000000  // Mask Message Direction
#define CAN_IFMSK_IDMSK_M      0x1FFFFFFF  // Identifier Mask
#define CAN_IFMSK_IDMSK_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the CAN_O_IF1ARB2 register.
//
//*****************************************************************************
#define CAN_IFARB_MSGVAL      0x80000000  // Message Valid
#define CAN_IFARB_XTD         0x40000000  // Extended Identifier
#define CAN_IFARB_DIR         0x20000000  // Message Direction
#define CAN_IFARB_ID_M        0x1FFFFFFF  // Message Identifier
#define CAN_IFARB_11ID_M      0x1FFC0000  // Message Identifier
#define CAN_IFARB_ID_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the IF1MCTL register.
//
//*****************************************************************************
#define CAN_IFMCTL_NEWDAT      0x00008000  // New Data
#define CAN_IFMCTL_MSGLST      0x00004000  // Message Lost
#define CAN_IFMCTL_INTPND      0x00002000  // Interrupt Pending
#define CAN_IFMCTL_UMASK       0x00001000  // Use Acceptance Mask
#define CAN_IFMCTL_TXIE        0x00000800  // Transmit Interrupt Enable
#define CAN_IFMCTL_RXIE        0x00000400  // Receive Interrupt Enable
#define CAN_IFMCTL_RMTEN       0x00000200  // Remote Enable
#define CAN_IFMCTL_TXRQST      0x00000100  // Transmit Request
#define CAN_IFMCTL_EOB         0x00000080  // End of Buffer
#define CAN_IFMCTL_DLC_M       0x0000000F  // Data Length Code
#define CAN_IFMCTL_DLC_S       0


void ConfigureCANDevice(canBASE_t *node);
void AppUpdaterCAN(void);
void UpdaterCAN(canBASE_t *node);
static void PacketWrite(canBASE_t *node, uint32_t ulId, const uint8_t *pucData, uint32_t ulSize);
static uint32_t PacketRead(canBASE_t *node, uint8_t *pucData, uint32_t *pulSize);
static void ConfigureCANInterface(canBASE_t *node, uint32_t ulSetTiming);
static void CANMessageSetTx(canBASE_t *node, uint32_t ulId, const uint8_t *pucData,uint32_t ulSize);
static uint32_t CANMessageGetRx(canBASE_t *node, uint8_t *pucData, uint32_t *pulMsgID);
static void CANMessageSetRx(canBASE_t *node);

#endif // __BL_CAN_H__
