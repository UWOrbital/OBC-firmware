#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

#include <command.h>

//#include <timekeeper_sg.h>

QueueHandle_t command_queue;

void push_command(cmd_msg_t cmd) {

    BaseType_t xTask;

    // Check if the command has a time tag
    if (cmd.timestamp > 0) {
        // If it does, add it to the command queue and execute it later
        xTask = xQueueSend(command_queue, &cmd, 0); //should this be &cmdTmp or just cmdTmp
        if (xTask != pdPASS) {
            printf("Error: command queue full\n");
        }
    } 
    else {
        // If it doesn't, execute it immediately
        //if i leave an else statement here will it move onto handle_command
        //handle_command()
        printf("Executing command: %s\n", cmd.id);
    }

    /*
    BaseType_t result = xQueueSend(command_queue, &cmdTmp, 0);
    
    if (result == pdPASS) {
        printf("Command added to queue\n");
    } else {
        printf("Failed to add command to queue: error %ld\n", result);
    }
    */
}

void execute_command(cmd_msg_t cmd) {
    printf("Executing command: commandName = %s\n, timeTag=%d\n, inclination=%d\n, RAAN=%d\n, eccentricity=%d\n, argPerigee=%d\n, meanAnomaly=%d\n, meanMotion=%d\n, numRevolution=%d\n", commandTemplate.commandName, commandTemplate.timeTag, commandTemplate.inclination, commandTemplate.RAAN, commandTemplate.eccentricity, commandTemplate.argPerigee, commandTemplate.meanAnomaly, commandTemplate.meanMotion, commandTemplate.numRevolution); //is this necessary?
}

void handle_command(void *pvParameters) {

    //commandTemplate cmdTmp;
    cmd_msg_t cmd;

    for (;;) { //or while(1)
        
        //get current time from getCurrentTime
        int current_time = getCurrentTime;
        xQueueReceive(command_queue, &cmd, portMAX_DELAY); //shoud the time the task can be blocked be portMAX_DELAY?
        uint8_t cmdId = cmd.id;

        if (cmd.isTimeTagged == true ) {
            if (current_time = cmd.timestamp) {
                // if current time is equal to the timetag, execute the command.
                printf("Timetag of %s\n is equal than current time of %d\n. Executing command: %s\n", cmdTmp.commandName, current_time, cmdTmp.commandName); //fix this based on if timetag is before current time
                continue;

            }
            if (current_time > cmd.timestamp) {
                //throw away the command. how to throw away command?
                printf("Timetag of %s\n is smaller than current time of %d\n. Throwing away command: %s\n", cmdTmp.commandName, current_time, cmdTmp.commandName); //fix this based on if timetag is before current time

            }
            else { //if (current_time < cmdTmp.timeTag) 
                // Put the command back into the queue if the time tag has not been reached
                xQueueSendToFront(command_queue, &cmd, 0);
                vTaskDelayUntil(&current_time, pdMS_TO_TICKS(1000));
                continue;
            }
        }
        
        switch(cmdId) {
				case CMD_TLE_DATA_UPLINK:
					tleDataUplinkCmdCallback(cmd.tleDataUplink);
					break;

                case CMD_ARO_DATA_UPLINK:
                    aroDataUplinkCmdCallback(cmd.aroDataUplink);
                    break;

                case CMD_RTC_SYNC:
                    rtcSyncCmdCallback(cmd.rtcSync);
                    break;

                case CMD_SECONDARY_PAYLOAD_ACTIVATION:
                    secondaryPayloadActivationCmdCallback( cmd.secondaryPayloadActivationTime);
                    break;
                
                case CMD_CALL_EPS_MCU_RESET:
                    callEPSMCUResetCmdCallback( cmd.callEPSMCUReset );
                    break;

                case CMD_CALL_CDH_MCU_RESET:
                    callCDHMCUResetCmdCallback( cmd.callCDHMCUReset );
                    break;

                case CMD_TIME_DELAY_IMG_CAPTURE_AND_DOWNLINK:
                    timeDelayImgCaptureAndDownlinkCmdCallback( cmd.timeDelayImgCaptureAndDownlink);
                    break;
                
                case CMD_DIRECT_EPS_POWER_RAIL_CONTROL:
                    directEPSPowerRailControlCmdCallback( cmd.directEPSPowerRailControl );
                    break;

                case CMD_COMMS_ECHO_BACK:
                    commsEchoBackCmdCallback( cmd.commsEchoBack );
                    break;
                
                case CMD_CHANGE_VALUE_PARAM_MNGER:
                    changeValueParmMngerCmdCallback( cmd.changeValueParmMnger );
                    break;

                case CMD_ECHO_PARM_MNGER_VALUE:
                    echoParmMngerValueCmdCallback( cmd.echoParmMngerValue );
                    break;

                case CMD_MANUALLY_ACTIVATE_EMERGENCY_MODE_SPECIFIED_TIME:
                    manuallyActivateEmergencyModeSpecifiedTimeCmdCallback( cmd.manuallyActivateEmergencyModeSpecifiedTime );
                    break;

                case CMD_DOWNLINK_FULL_SYS_LOGS_NEXT_PASS:
                    downlinkFullSysLogsNextPassCmdCallback( cmd.downlinkFullSysLogsNextPass );
                    break;

                //should I add all these switch cases into execute_command?

        /*
         if (cmdTmp.timeTag > 0) {

            if (current_time >= cmdTmp.timeTag) {
                // if current time is greater or equal than the time in the timetag, execute the command.
                //should the > be included? how should we handle commands that were set at a time before the current time. Is there any case where somehow commands are being taken in later than when they're sent, and also is there any case/protocol where we don't want to take in any commands that were set to execute earlier than current time.
                printf("Timetag of %s\n is smaller than current time of %d\n. Executing command: %s\n", cmdTmp.commandName, current_time, cmdTmp.commandName); //fix this based on if timetag is before current time

            }
            else { //if (current_time < cmdTmp.timeTag) 
                // Put the command back into the queue if the time tag has not been reached
                xQueueSendToFront(command_queue, &cmdTmp, 0);
                vTaskDelayUntil(&current_time, pdMS_TO_TICKS(1000));
                continue;
            }
        }
        */
                    
        }

        execute_command(cmd);

    }
}

int main(void) {
    
    //creating the command queue
    command_queue = xQueueCreate(COMMAND_QUEUE_LENGTH, sizeof(commandTemplate));

    //creating the command task
    xTaskCreate(handle_command, "commandTemplate Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL); //what should stack size be. who handles the task, as it is set to NULL right now. what should the task priority be? Should we set priority as: (tskIDLE_PRIORITY)? check tskIDLE_PRIORITY and see if it needs +1

    //FreeRTOS scheduler
    vTaskStartScheduler();

    return 0;

    /*
    //some dummy placeholder commands for now
    commandTemplate cmd1 = {"Rotate", 0, 20, 0, 0, 0, 0, 0, 0}; //no time tag
    push_command(cmd1);

    commandTemplate cmd2 = {"Bound", pdMS_TO_TICKS(5000), 20, 0, 0, 0, 0, 0, 0}; //should I use pdMS_TO_TICKS(5000), or just provide a seconds value. what should be passed in the commandTemplate
    push_command(cmd2);
    */
}
