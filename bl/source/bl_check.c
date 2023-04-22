//*****************************************************************************
//
// bl_check.c: Code to check for a forced update.
// Author    : QJ Wang. qjwang@ti.com
// Date      : 4-18-2019
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
#include "bl_check.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_gio.h"
#include "HL_sys_common.h"
#else
#include "gio.h"
#include "sys_common.h"
#endif
//*****************************************************************************
//
//  bl_check_api
//
//*****************************************************************************

//*****************************************************************************
//
// This global is used to remember if a forced update occurred.
//
//*****************************************************************************
extern unsigned int g_pulUpdateSuccess[8];
extern unsigned int g_ulUpdateStatusAddr;


#if (FORCED_UPDATE_PORT == GPIO_PORTA_BASE)
#define gioPORT gioPORTA
#else
#define gioPORT gioPORTB
#endif


//*****************************************************************************
//
// Checks a GPIO for a forced update.
//
// This function checks the state of a GPIO to determine if a update is being
// requested.
//
// \return Returns a non-zero value if an update is being requested and zero
// otherwise.
//
//*****************************************************************************
#ifdef ENABLE_UPDATE_CHECK
uint32_t
CheckGPIOForceUpdate(void)
{
    /** bring GIO module out of reset */
    gioREG->GCR0      = 1;
    gioREG->ENACLR = 0xFF;
    gioREG->LVLCLR    = 0xFF;  

    // Set the pin as input
    gioPORT->DIR &= ~(1 << FORCED_UPDATE_PIN);

    // Enable the pull up/down.
    gioPORT->PULDIS &= ~(1 << FORCED_UPDATE_PIN);

    // Enable the weak pull up.
    gioPORT->PSL |= 1 << FORCED_UPDATE_PIN;

    // Check the pin to see if an update is being requested.

    if ((gioPORT->DIN & (0x1 << FORCED_UPDATE_PIN) ) == 0)
    {
        // Remember that this was a forced update.
        return(1);
    }

    // No update is being requested so return 0.
    return(0);
}
#endif

//*****************************************************************************
//
// Checks if an update is needed or is being requested.
//
// This function detects if an update is being requested or if there is no
// valid code presently located on the microcontroller.  This is used to tell
// whether or not to enter update mode.
//
// \return Returns a non-zero value if an update is needed or is being
// requested and zero otherwise.
//
//*****************************************************************************
uint32_t
CheckForceUpdate(void)
{
    uint32_t *pulApp;

    //
    // See if the first word of status section is 0x5A5A5A5A. If it is not 0x5A5A5A5A ,
    // there is no application code in the flash. The update is required
    //
    pulApp = (uint32_t *)g_ulUpdateStatusAddr;
    if((pulApp[0] != g_pulUpdateSuccess[0]))
    {
        return(1);    //1 means Need New UPDATE
    }

// if the checked location is 0x5A5A5A5A, check GPIO forced update
#ifdef ENABLE_UPDATE_CHECK
    // If simple GPIO checking is configured, determine whether or not to force an update.
    return(CheckGPIOForceUpdate());
#else
    // GPIO checking is not required so, if we get here, a valid image exists and no update is needed.
    return(0);
#endif

}

