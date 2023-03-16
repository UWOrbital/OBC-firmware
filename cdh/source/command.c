#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

#include <timekeeper_sg.h>

//#include<ds3232_mz.c> //file for the RTC alarm functions


//COMMS creates a function that sets x to a command, and returns x, of data type they choose called maybe commandDataType
//Now i can access x, and find x.timeTag, then store and sort

//check message buffer instead of making a queue everytime executing command with time tag

//need if statement for looking through stored tasks and setting an alarm for most recent task and when the task is up, set the case in swtich as that task
//implement command priority?
//if time tagged command, we expect a time value for when it should be run.
//time tag will be a value, if time tag value is zero, then do immediately, if any other number, then that number
//if statement to find if there is a value != 0 in the time tag from the struct command. 
//store in FRAM and RTC alarm system. Talk to daniel
//maybe have a global array where we can store the commands to spoo
//function getTimeTag that passes in the function

//have one task that deals with alarms, maybe even before finishing this up
//time service is able to set an alarm. Talk to shourrya about this

#define COMMAND_QUEUE_LENGTH 10
#define COMMAND_QUEUE_SIZE sizeof(commandTemplate)

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
                printf("Timetag of %s\n is smaller than current time of %d\n. Executing command: %s\n", cmdTmp.commandName, current_time, cmdTmp.commandName);


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
