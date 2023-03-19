#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

#include <timekeeper_sg.h>

/*command IDs*/
#define NUM_CMDS 14
#define CMD_TLE_DATA_UPLINK (uint8_t) 1 //based on the google sheets, first command starts as ID = 1. Should it be 0 instead?
#define CMD_ARO_DATA_UPLINK (uint8_t) 2
#define CMD_SECONDARY_PAYLOAD_ACTIVATION (uint8_t) 3
#define CMD_RTC_SYNC (uint8_t) 4
#define CMD_SECONDARY_PAYLOAD_ACTIVATION_TIME (uint8_t) 5
#define CMD_CALL_EPS_MCU_RESET (uint8_t) 6
#define CMD_CALL_CDH_MCU_RESET (uint8_t) 7
#define CMD_TIME_DELAY_IMG_CAPTURE_AND_DOWNLINK (uint8_t) 8
#define CMD_DIRECT_EPS_POWER_RAIL_CONTROL (uint8_t) 9
#define CMD_COMMS_ECHO_BACK (uint8_t) 10
#define CMD_CHANGE_VALUE_PARAM_MNGER (uint8_t) 11
#define CMD_ECHO_PARM_MNGER_VALUE (uint8_t) 12
#define CMD_MANUALLY_ACTIVATE_EMERGENCY_MODE_SPECIFIED_TIME (uint8_t) 13
#define CMD_DOWNLINK_FULL_SYS_LOGS_NEXT_PASS (uint8_t) 14

#define COMMAND_QUEUE_LENGTH 10
#define COMMAND_QUEUE_SIZE sizeof(commandTemplate)

//#include<ds3232_mz.c> //file for the RTC alarm functions
//check message buffer instead of making a queue everytime executing command with time tag
//store in FRAM and RTC alarm system. Talk to daniel

/*data structs*/

typedef struct {
    char commandName[30];
    int timeTag;
	float inclination;
	float RAAN;
	float eccentricity;
	float argPerigee;
	float meanAnomaly;
	double meanMotion;
	int numRevolution;
} commandTemplate;

// CMD_TLE_DATA_UPLINK
typedef struct {
	float inclination;
	float raan;
	float eccentricity;
	float argOfPedigree;
	float meanAnomaly;
	double meanMotion;
	uint32_t revolution;
} tle_data_uplink_t; //need to ask why the best practice is to put a "_t" at the end

//CMD_ARO_DATA_UPLINK
typedef struct {
    uint32_t timeTag;
    float rollAngle; //why is there "off nadir" in the google sheets?
    //ARO Callsign?
    char passcorde;
} aro_data_uplink_t;

//CMD_SECONDARY_PAYLOAD_ACTIVATION
typedef struct{

} secondary_payload_activation_t;

// CMD_RTC_SYNC 
typedef struct {
	uint32_t time;
} rtc_sync_t;

//CMD_SECONDARY_PAYLOAD_ACTIVATION_TIME
typedef struct {
    uint32_t timeTag;
} secondary_payload_activation_time_t;

//CMD_CALL_EPS_MCU_RESET
typedef struct {

} call_eps_mcu_reset_t;

//CMD_CALL_CDH_MCU_RESET
typedef struct {

} call_cdh_mcu_reset_t;

//CMD_TIME_DELAY_IMG_CAPTURE_AND_DOWNLINK
typedef struct {
    uint32_t timeTag;
    float rollAngle;
} time_delay_img_capture_and_downlink_t;

//CMD_DIRECT_EPS_POWER_RAIL_CONTROL
typedef struct {
    int powerRailID;
} direct_eps_power_rail_control_t;

//CMD_COMMS_ECHO_BACK
typedef struct {
    uint32_t intEchoBack;
} comms_echo_back_t;

//CMD_CHANGE_VALUE_PARAM_MNGER
typedef struct {
    int newAddress;
} change_value_parm_mnger_t;

//CMD_ECHO_PARM_MNGER_VALUE
typedef struct {
    int address;
} echo_parm_mnger_value_t;

//CMD_MANUALLY_ACTIVATE_EMERGENCY_MODE_SPECIFIED_TIME
typedef struct {
    int emergModeNum;
    uint32_t timeTag;
} manually_activate_emergency_mode_specified_time_t;

//CMD_DOWNLINK_FULL_SYS_LOGS_NEXT_PASS
typedef struct {
    enum logLevel;
} downlink_full_sys_logs_next_pass_t;

/*data structs*/

/*command struct*/
typedef struct {
	union {
		tle_data_uplink_t tleDataUplink;
		aro_data_uplink_t aroDataUplink;
        secondary_payload_activation_t secondayPayloadActivation;
        rtc_sync_t rtcSync;
        secondary_payload_activation_time_t secondaryPayloadActivationTime;
        call_eps_mcu_reset_t callEPSMCUReset;
        call_cdh_mcu_reset_t callCDHMCUReset;
		time_delay_img_capture_and_downlink_t timeDelayImgCaptureAndDownlink;
        direct_eps_power_rail_control_t directEPSPowerRailControl;
        comms_echo_back_t commsEchoBack;
        change_value_parm_mnger_t changeValueParmMnger;
        echo_parm_mnger_value_t echoParmMngerValue;
        manually_activate_emergency_mode_specified_time_t manuallyActivateEmergencyModeSpecifiedTime;
        downlink_full_sys_logs_next_pass_t downlinkFullSysLogsNextPass;
	};
	uint32_t timestamp;
	uint8_t id;
	bool isTimeTagged;
} cmd_msg_t;
/*command struct*/

// Callback functions (Will probably be implemented in other files)
void tleDataUplinkCmdCallback(tle_data_uplink_t tleDataUplink) {
	// This is just an example of what the callback could be used for
	sendToADCSQueue(tleDataUplink);
}

void aroDataUplinkCmdCallback(aro_data_uplink_t aroDataUplink) {
    sendToADCSQueue(aroDataUplink);
	
}

void secondaryPayloadActivationCmdCallback(...) {
	...
}


QueueHandle_t command_queue;

void push_command(commandTemplate cmdTmp) {

    BaseType_t xTask;

    // Check if the command has a time tag
    if (cmdTmp.timeTag > 0) {
        // If it does, add it to the command queue and execute it later
        xTask = xQueueSend(command_queue, &cmdTmp, 0); //should this be &cmdTmp or just cmdTmp
        if (xTask != pdPASS) {
            printf("Error: command queue full\n");
        }
    } 
    else {
        // If it doesn't, execute it immediately
        printf("Executing command: %s\n", cmdTmp.command);
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

void execute_command(commandTemplate cmdTmp) {
    //print the values of each variable in the unique command
    //send event to whichever sub team needs it, and the struct in it too. 
    //will who I send the event to be based on the command ID? 
    //What does it means to send an event to another subteam?
    printf("Executing command: commandName = %s\n, timeTag=%d\n, inclination=%d\n, RAAN=%d\n, eccentricity=%d\n, argPerigee=%d\n, meanAnomaly=%d\n, meanMotion=%d\n, numRevolution=%d\n", commandTemplate.commandName, commandTemplate.timeTag, commandTemplate.inclination, commandTemplate.RAAN, commandTemplate.eccentricity, commandTemplate.argPerigee, commandTemplate.meanAnomaly, commandTemplate.meanMotion, commandTemplate.numRevolution); //is this necessary?
}

void handle_command(void *pvParameters) {

    commandTemplate cmdTmp;

    for (;;) { //is there any difference if i use while(1)
        
        //get current time from getCurrentTime
        int current_time = getCurrentTime;
        xQueueReceive(command_queue, &cmdTmp, portMAX_DELAY); //shoud the time the task can be blocked be portMAX_DELAY?

        if (cmdTmp.timeTag > 0) {

            if (current_time >= cmdTmp.timeTag) {
                // if current time is greater or equal than the time in the timetag, execute the command.
                //should the > be included? how should we handle commands that were set at a time before the current time. Is there any case where somehow commands are being taken in later than when they're sent, and also is there any case/protocol where we don't want to take in any commands that were set to execute earlier than current time.
                printf("Timetag of %s\n is smaller than current time of %d\n. Executing command: %s\n", cmdTmp.commandName, current_time, cmdTmp.commandName); //fix this based on if timetag is before current time


            else { //if (current_time < cmdTmp.timeTag) 
                // Put the command back into the queue if the time tag has not been reached
                xQueueSendToFront(command_queue, &cmdTmp, 0);
                vTaskDelayUntil(&current_time, pdMS_TO_TICKS(1000));
                continue;
            }
        }

        execute_command(cmdTmp);

    }
}

int main(void) {
    
    //creating the command queue
    command_queue = xQueueCreate(COMMAND_QUEUE_LENGTH, sizeof(Command));

    //creating the command task
    xTaskCreate(handle_command, "commandTemplate Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL); //what should stack size be. who handles the task, as it is set to NULL right now. what should the task priority be? Should we set priority as: (tskIDLE_PRIORITY)? check tskIDLE_PRIORITY and see if it needs +1

    //some dummy placeholder commands for now
    commandTemplate cmd1 = {"Rotate", 0, 20, 0, 0, 0, 0, 0, 0}; //no time tag
    push_command(cmd1);

    commandTemplate cmd2 = {"Bound", pdMS_TO_TICKS(5000), 20, 0, 0, 0, 0, 0, 0}; //should I use pdMS_TO_TICKS(5000), or just provide a seconds value. what should be passed in the commandTemplate
    push_command(cmd2);

    //FreeRTOS scheduler
    vTaskStartScheduler();

    return 0;
}
