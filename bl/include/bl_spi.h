//*****************************************************************************
//
// bl_spi.h - Definitions for the SPI transport functions.
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

#ifndef __BL_SPI_H__
#define __BL_SPI_H__


#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_spi.h"
#include "HL_sys_common.h"
#else
#include "spi.h"
#include "sys_common.h"
#endif

//*****************************************************************************
//
// SPI Transport APIs
//
//*****************************************************************************
extern void SPISend(spiBASE_t *node, uint8_t const *pucData, uint8_t ulSize);
extern void SPIReceive(spiBASE_t *node, uint8_t *pucData, uint8_t ulSize);
void UpdaterSPI(spiBASE_t *node);
void ConfigureSPIDevice(spiBASE_t *node);

//*****************************************************************************
//
// Define the transport functions if the SPI port is being used.
//
//*****************************************************************************
#ifdef SPI_ENABLE_UPDATE
#define SendData                SPISend
#define ReceiveData             SPIReceive
#endif

#endif // __BL_SPI_H__
