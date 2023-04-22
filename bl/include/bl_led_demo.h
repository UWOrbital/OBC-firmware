//*****************************************************************************
//
// bl_led_demo.h - Definitions for the Liting LEDs
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

#ifndef BL_LED_DEMO_H_
#define BL_LED_DEMO_H_

#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_sys_common.h"
#else
#include "sys_common.h"
#endif


void run_LED_StartUp(uint8_t ledNum);

/* LEDs on TI HDKs */
#if defined(TMS570LS04) || defined(RM42)

#define LITE_TOPRIGHT_LED    run_LED_StartUp(3)
#define LITE_BOTRIGHT_LED    run_LED_StartUp(2)
#define LITE_TOPLEFT_LED     run_LED_StartUp(6)
#define LITE_BOTLEFT_LED     run_LED_StartUp(4)

#else

#define LITE_TOPRIGHT_LED    run_LED_StartUp(0)
#define LITE_BOTRIGHT_LED    run_LED_StartUp(25)
#define LITE_TOPLEFT_LED     run_LED_StartUp(17)
#define LITE_BOTLEFT_LED     run_LED_StartUp(29)

#endif


#endif /* BL_LED_DEMO_H_ */
