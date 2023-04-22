//
// bl_led_demo.c  : Flash the leds
// Author         : QJ Wang. qjwang@ti.com
// Date           : 4-11-2019
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


#include "bl_led_demo.h"
#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_het.h"
#include "HL_gio.h"
#include "HL_sys_common.h"
#else
#include "het.h"
#include "gio.h"
#include "sys_common.h"
#endif


/** @fn run_LED_StartUp(void)
*   
*   This function is called during the start up, LED's are turned on 
*   in a sequence to indicate the board is powerd up and ready to run
*   the demo software.  
*   
*/



#if defined(TMS570LS04) || defined(RM42)

void run_LED_StartUp(uint8_t ledNum)
{
   	int temp,delay;

   	gioInit();

	/** - Delay Parameter */
	delay				= 0x40000;

// LED: GIOA6           GIOA3
// LED: GIOA4           GIOA2
//
	if(1)
	{
	    gioPORTA->DIR  =  0        /* Bit 0 */
	                   | (0 << 1)  /* Bit 1 */
	                   | (1 << 2)  /* Bit 2 */
	                   | (1 << 3)  /* Bit 3 */
	                   | (1 << 4)  /* Bit 4 */
	                   | (0 << 5)  /* Bit 5 */
	                   | (1 << 6)  /* Bit 6 */
	                   | (0 << 7); /* Bit 7 */

			/** - Turn all LEDs off */
			gioPORTA->DOUT  =  0x00;

			/** - Simple Dealy */
			for(temp=0;temp<delay;temp++);

			/** - Set only GIO[3] */
			gioPORTA->DOUT  = (0x01 << ledNum);;
			/** - Simple Dealy */
			for(temp=0;temp<delay;temp++);

	    }

	}

#else  //for TMS570LS31x/12x RM48/46

void run_LED_StartUp(uint8_t ledNum)
{
   	int temp,delay;
   	//LED Number: 0, 31, 17, 29, 18, 25
	/** - Delay Parameter */
	delay = 0x20000;

	/** - This if statment is a placeholder for ArgoBoard/USBStick check */
	if(1)
	{
		/** - Configure NHET pins as output */
   		hetREG1->DIR = 0 | (1 << 0)
   				        | (1 << 5)
   				        | (1 << 17)
   				        | (1 << 18)
   				        | (1 << 25)
   				        | (1 << 27)
   				        | (1 << 29)
   				        | (1 << 31);


		/** - Turn all LEDs off */
		hetREG1->DOUT   	= 0x00000000;

		/** - Simple Delay */
		for(temp=0;temp<delay;temp++);

		/** - Set only NHET[0] */ 
		hetREG1->DOUT   	= (0x1 << ledNum);
		/** - Simple Delay */
		for(temp=0;temp<delay;temp++);
		
	}
}
#endif
