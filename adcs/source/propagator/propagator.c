#include "propagator/propagator.h"
#include "console.h"

// These are from the SGP4 implementation
#include "TLE.h"
#include <SGP4.h>

#include <FreeRTOS.h>
#include <os_task.h>

#include <os_message_buffer.h>

#include <string.h>

extern MessageBufferHandle_t tlePropagatorMsgBuffer;

tle_msg_t recv_tle_msg;

TLE curr_tle;
bool flag_recv_curr_tle = false;

void displayPosVel(double *r, double *v){
    printConsole("Position: x: %lf\ty: %lf\tz: %lf\n", r[0], r[1], r[2]);
    printConsole("Velocity: x: %lf\ty: %lf\tz: %lf\n", v[0], v[1], v[2]);
}


void vOrbitPropagatorTask(void * pvParameters){

    
    // Infinite loop

    while(1){
        size_t recvBytes = 0;
        recvBytes = xMessageBufferReceive(tlePropagatorMsgBuffer, &recv_tle_msg, sizeof(tle_msg_t), TLE_PROPAGATOR_MSG_BUFFER_WAIT_TICKS);

        if(recvBytes > 0){
            printConsole("Received %i bytes...\n", recvBytes);

            // Parses the current TLE and sets flag
            parseLines(&curr_tle, recv_tle_msg.line_1, recv_tle_msg.line_2);  
            flag_recv_curr_tle = true; 
        }

        if(flag_recv_curr_tle){
            // Since TLE data has been received, is able to calculate radius and velocity
            
            // Position and velocity
            double r[3], v[3];

            unsigned long start = xTaskGetTickCount();

            // Calculates milliseconds from ticks -> calculates minutes
            getRV(&curr_tle, (double)(xTaskGetTickCount() * portTICK_RATE_MS) / 60000.0, r, v);
            displayPosVel(r, v);
            // printConsole("Error: %i\n", curr_tle.sgp4Error);

            unsigned long end = xTaskGetTickCount();

            printConsole("SGP4 computation took %i ticks...\n", (end - start));
            
        } else{
            printConsole("Unable to calculate position and velocity as no TLE...\n");
        }

        vTaskDelay(2000);
    };
}