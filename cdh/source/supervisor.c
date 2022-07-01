/*
 * supervisor.c
 *
 * May 30, 2022
 * kiransuren
 *
 */

#include "supervisor.h"
#include "obc_sci_comms.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#include "sys_common.h"
#include "gio.h"
#include "sci.h"

void vSupervisorTask(void * pvParameters){
    while(1){
        char message[] = "Supervisor has toggled GPIO.\r\n";
        gioToggleBit(gioPORTB, 1);
        sci_send_text(scilinREG,(uint8*)message, sizeof(message));
        vTaskDelay(SUPERVISOR_DELAY_TICKS * 2);
    }
}