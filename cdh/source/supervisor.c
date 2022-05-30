#include "supervisor.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#include "sys_common.h"
#include "gio.h"

void vSupervisorTask(void * pvParameters){

    while(1){
        gioToggleBit(gioPORTB, 1);
        vTaskDelay(SUPERVISOR_DELAY_TICKS);
    }
}

