/*
 * supervisor.h
 *
 * May 30, 2022
 * kiransuren
 *
 */


#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#define SUPERVISOR_STACK_SIZE   1024
#define SUPERVISOR_NAME         "supervisor"
#define SUPERVISOR_PRIORITY     1
#define SUPERVISOR_DELAY_TICKS  1000/portTICK_PERIOD_MS

void vSupervisorTask(void * pvParameters);


#endif
