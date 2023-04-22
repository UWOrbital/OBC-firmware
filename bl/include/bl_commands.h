//*****************************************************************************
//
// bl_commands.h - The list of commands and return messages supported by the
//                 boot loader.
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

#ifndef __BL_COMMANDS_H__
#define __BL_COMMANDS_H__

//*****************************************************************************
// These defines are used to define the range of values that are used for
// CAN update protocol.
//*****************************************************************************
#define CAN_MSGID_MFR           0x5A0

//*****************************************************************************
// The Hercules Firmware Update API definitions.
//*****************************************************************************
#define CAN_API_UPD                     CAN_MSGID_MFR                  //0x5a0
#define CAN_COMMAND_PING               (CAN_API_UPD | 0) << 0         //0x5a1
#define CAN_COMMAND_GET_ADDR_SIZE      (CAN_API_UPD | 1) << 0         //0x5a2
#define CAN_COMMAND_RUN                (CAN_API_UPD | 2) << 0         //0x5a3
#define CAN_COMMAND_GET_DATA           (CAN_API_UPD | 4) << 0         //0x5a5
#define CAN_COMMAND_RESET              (CAN_API_UPD | 5) << 0         //0x5a6
#define CAN_COMMAND_ACK                (CAN_API_UPD | 6) << 0         //0x5a7


//*****************************************************************************
//
// This command is used to receive an acknowledge from the the boot loader
// proving that communication has been established.  This command is a single
// byte.
//
// The format of the command is as follows:
//
//     uint8_t ucCommand[1];
//
//     ucCommand[0] = COMMAND_PING;
//
//*****************************************************************************
#define SPI_COMMAND_PING            0x20

//*****************************************************************************
//
// This command is sent to the boot loader to indicate where to store data and
// how many bytes will be sent by the COMMAND_SEND_DATA commands that follow.
// The command consists of two 32-bit values that are both transferred MSB
// first.  The first 32-bit value is the address to start programming data
// into, while the second is the 32-bit size of the data that will be sent.
// This command also triggers an erasure of the full application area in the
// flash or possibly the entire flash depending on the address used.  This
// causes the command to take longer to send the ACK/NAK in response to the
// command.  This command should be followed by a COMMAND_GET_STATUS to ensure
// that the program address and program size were valid for the microcontroller
// running the boot loader.
//
// The format of the command is as follows:
//
//     uint8_t ucCommand[9];
//
//     ucCommand[0] = COMMAND_DOWNLOAD;
//     ucCommand[1] = Program Address [31:24];
//     ucCommand[2] = Program Address [23:16];
//     ucCommand[3] = Program Address [15:8];
//     ucCommand[4] = Program Address [7:0];
//     ucCommand[5] = Program Size [31:24];
//     ucCommand[6] = Program Size [23:16];
//     ucCommand[7] = Program Size [15:8];
//     ucCommand[8] = Program Size [7:0];
//
//*****************************************************************************
#define SPI_COMMAND_DOWNLOAD        0x21

//*****************************************************************************
//
// This command is sent to the boot loader to transfer execution control to the
// specified address.  The command is followed by a 32-bit value, transferred
// MSB first, that is the address to which execution control is transferred.
//
// The format of the command is as follows:
//
//     uint8_t ucCommand[5];
//
//     ucCommand[0] = COMMAND_RUN;
//     ucCommand[1] = Run Address [31:24];
//     ucCommand[2] = Run Address [23:16];
//     ucCommand[3] = Run Address [15:8];
//     ucCommand[4] = Run Address [7:0];
//
//*****************************************************************************
#define SPI_COMMAND_RUN             0x22

//*****************************************************************************
//
// This command returns the status of the last command that was issued.
// Typically this command should be received after every command is sent to
// ensure that the previous command was successful or, if unsuccessful, to
// properly respond to a failure.  The command requires one byte in the data of
// the packet and the boot loader should respond by sending a packet with one
// byte of data that contains the current status code.
//
// The format of the command is as follows:
//
//     uint8_t ucCommand[1];
//
//     ucCommand[0] = COMMAND_GET_STATUS;
//
// The following are the definitions for the possible status values that can be
// returned from the boot loader when <tt>COMMAND_GET_STATUS</tt> is sent to
// the microcontroller.
//
//     COMMAND_RET_SUCCESS
//     COMMAND_RET_UNKNOWN_CMD
//     COMMAND_RET_INVALID_CMD
//     COMMAND_RET_INVALID_ADD
//     COMMAND_RET_FLASH_FAIL
//
//*****************************************************************************
#define SPI_COMMAND_GET_STATUS      0x23

//*****************************************************************************
//
// This command should only follow a COMMAND_DOWNLOAD command or another
// COMMAND_SEND_DATA command, if more data is needed.  Consecutive send data
// commands automatically increment the address and continue programming from
// the previous location.  The transfer size is limited by the size of the
// receive buffer in the boot loader (as configured by the BUFFER_SIZE
// parameter).  The command terminates programming once the number of bytes
// indicated by the COMMAND_DOWNLOAD command has been received.  Each time this
// function is called, it should be followed by a COMMAND_GET_STATUS command to
// ensure that the data was successfully programmed into the flash.  If the
// boot loader sends a NAK to this command, the boot loader will not increment
// the current address to allow retransmission of the previous data.
//
// The format of the command is as follows:
//
//     uint8_t ucCommand[9];
//
//     ucCommand[0] = COMMAND_SEND_DATA
//     ucCommand[1] = Data[0];
//     ucCommand[2] = Data[1];
//     ucCommand[3] = Data[2];
//     ucCommand[4] = Data[3];
//     ucCommand[5] = Data[4];
//     ucCommand[6] = Data[5];
//     ucCommand[7] = Data[6];
//     ucCommand[8] = Data[7];
//
//*****************************************************************************
#define SPI_COMMAND_SEND_DATA       0x24

//*****************************************************************************
//
// This command is used to tell the boot loader to reset.  This is used after
// downloading a new image to the microcontroller to cause the new application
// or the new boot loader to start from a reset.  The normal boot sequence
// occurs and the image runs as if from a hardware reset.  It can also be used
// to reset the boot loader if a critical error occurs and the host device
// wants to restart communication with the boot loader.
//
// The format of the command is as follows:
//
//     uint8_t ucCommand[1];
//
//     ucCommand[0] = COMMAND_RESET;
//
// The boot loader responds with an ACK signal to the host device before
// actually executing the software reset on the microcontroller running the
// boot loader.  This informs the updater application that the command was
// received successfully and the part will be reset.
//
//*****************************************************************************
#define SPI_COMMAND_RESET           0x25

//*****************************************************************************
//
// This is returned in response to a COMMAND_GET_STATUS command and indicates
// that the previous command completed successful.
//
//*****************************************************************************
#define SPI_COMMAND_RET_SUCCESS     0x40

//*****************************************************************************
//
// This is returned in response to a COMMAND_GET_STATUS command and indicates
// that the command sent was an unknown command.
//
//*****************************************************************************
#define SPI_COMMAND_RET_UNKNOWN_CMD 0x41

//*****************************************************************************
//
// This is returned in response to a COMMAND_GET_STATUS command and indicates
// that the previous command was formatted incorrectly.
//
//*****************************************************************************
#define SPI_COMMAND_RET_INVALID_CMD 0x42

//*****************************************************************************
//
// This is returned in response to a COMMAND_GET_STATUS command and indicates
// that the previous download command contained an invalid address value.
//
//*****************************************************************************
#define SPI_COMMAND_RET_INVALID_ADR 0x43

//*****************************************************************************
//
// This is returned in response to a COMMAND_GET_STATUS command and indicates
// that an attempt to program or erase the flash has failed.
//
//*****************************************************************************
#define SPI_COMMAND_RET_FLASH_FAIL  0x44

//*****************************************************************************
//
// This is the value that is sent to acknowledge a packet.
//
//*****************************************************************************
#define SPI_COMMAND_ACK             0x66

//*****************************************************************************
//
// This is the value that is sent to not-acknowledge a packet.
//
//*****************************************************************************
#define SPI_COMMAND_NAK             0x33


#endif // __BL_COMMANDS_H__
