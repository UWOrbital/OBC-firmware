/** @file setup.h
*   @brief Setup Functions
*   @date 11-Nov-2022 
*   
* 
*  
*   Relevant to CDH
*/

#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>

void funcSetUp()
{
    gioInit();
    sciInit();
    i2cInit();
    sciSetBaudrate( scilinREG, 115200 ); // Changed baud rate from 9600 to 115200
    sciSetBaudrate( sciREG, 115200 ); // Changed baud rate from 9600 to 115200
};


