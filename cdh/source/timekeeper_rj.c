/// first create a Queue using freeRTOS provided functions xqueuecreate, sample length of 20
///  

/// 1. Set Alarm : Direct function call in the driver: DONE
/// 2. Handle different alarms:
/// 3 and 4 ask daniel
/// 5. Set current time: Direct function call in driver: DONE
/// 6. Get current time: Direct function call in driver: DONE
/// 7. Assign a mutex to each function
/// 8. Reset function in RTC driver can be used to reset all current times
/// 9. Feature not developed yet in the driver due to RTC limitations

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <stdio.h>
#include <time.h>
#include <timekeeper_sg.h>
#include "ds3232_mz.h"

time_t currentTimekeepingTask(void *pvParameters)
{
    //time_t rtc_time = getCurrentTime; // The current time from the RTC. (CHECK IF THIS IS IN SECONDS)
    time_t rtc_time = time(NULL);
    time_t local_time = 0; // The current time in seconds
    uint32_t last_update_time = 0; // The time when we last updated the RTC time
    
    while(1){
        //Shourrya: We can create an increment function whi ch will handle the logic of incrementing hours, min and seconds in relation to each other
        local_time++; // Increment the local time value every second

        // Compare the local time with the RTC time every minute
        if (local_time - last_update_time >= 60) {
            if (local_time != rtc_time) {
                local_time = rtc_time; // Update the local time with the current time from the RTC
            }
            last_update_time = local_time;
        }
        
        return local_time;
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for a second?
    }
}




