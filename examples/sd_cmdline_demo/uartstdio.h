//*****************************************************************************
//
// uartstdio.h - Prototypes for the UART console functions.
//
// Copyright (c) 2007-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
//*****************************************************************************

#ifndef SD_CMDLINE_DEMO_UARTSTDIO_H_
#define SD_CMDLINE_DEMO_UARTSTDIO_H_

#include <stdarg.h>

extern int UARTgets(unsigned char *pcBuf, unsigned int ui32Len);
extern unsigned char UARTgetc(void);
extern int UARTwrite(unsigned char *pcBuf, unsigned int ui32Len);
extern void UARTprintf(unsigned char *pcString, ...);
extern void UARTvprintf(unsigned char *pcString, va_list vaArgP);

#endif // SD_CMDLINE_DEMO_UARTSTDIO_H_
