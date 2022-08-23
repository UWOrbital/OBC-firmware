#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION                       1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0
#define configUSE_IDLE_HOOK                        1
#define configUSE_TICK_HOOK                        0
#define configUSE_DAEMON_TASK_STARTUP_HOOK         0
#define configTICK_RATE_HZ                         ( 1000 )
#define configMINIMAL_STACK_SIZE                   ( ( unsigned short ) 70 )
#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 65 * 1024 ) )
#define configMAX_TASK_NAME_LEN                    ( 12 )
#define configUSE_TRACE_FACILITY                   1
#define configUSE_16_BIT_TICKS                     0
#define configIDLE_SHOULD_YIELD                    1
#define configUSE_MUTEXES                          1
#define configCHECK_FOR_STACK_OVERFLOW             0
#define configUSE_RECURSIVE_MUTEXES                1
#define configQUEUE_REGISTRY_SIZE                  20
#define configUSE_APPLICATION_TASK_TAG             1
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_ALTERNATIVE_API                  0
#define configUSE_QUEUE_SETS                       1
#define configUSE_TASK_NOTIFICATIONS               1
#define configSUPPORT_STATIC_ALLOCATION            1

#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY		3
#define configTIMER_QUEUE_LENGTH		10
#define configTIMER_TASK_STACK_DEPTH	configMINIMAL_STACK_SIZE

#define configMAX_PRIORITIES                       ( 7 )

unsigned long ulGetRunTimeCounterValue( void );
void vConfigureTimerForRunTimeStats( void );
#define configGENERATE_RUN_TIME_STATS             1

#define configUSE_CO_ROUTINES                     0
#define configMAX_CO_ROUTINE_PRIORITIES           ( 2 )

#define configUSE_STATS_FORMATTING_FUNCTIONS      0

#define configSTACK_DEPTH_TYPE                    uint32_t

#define INCLUDE_vTaskPrioritySet                  1
#define INCLUDE_uxTaskPriorityGet                 1
#define INCLUDE_vTaskDelete                       1
#define INCLUDE_vTaskCleanUpResources             0
#define INCLUDE_vTaskSuspend                      1
#define INCLUDE_vTaskDelayUntil                   1
#define INCLUDE_vTaskDelay                        1
#define INCLUDE_uxTaskGetStackHighWaterMark       1
#define INCLUDE_uxTaskGetStackHighWaterMark2      1
#define INCLUDE_xTaskGetSchedulerState            1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle    1
#define INCLUDE_xTaskGetIdleTaskHandle            1
#define INCLUDE_xTaskGetHandle                    1
#define INCLUDE_eTaskGetState                     1
#define INCLUDE_xSemaphoreGetMutexHolder          1
#define INCLUDE_xTimerPendFunctionCall            1
#define INCLUDE_xTaskAbortDelay                   1

#define configINCLUDE_MESSAGE_BUFFER_AMP_DEMO     0
#if ( configINCLUDE_MESSAGE_BUFFER_AMP_DEMO == 1 )
    extern void vGenerateCoreBInterrupt( void * xUpdatedMessageBuffer );
    #define sbSEND_COMPLETED( pxStreamBuffer )    vGenerateCoreBInterrupt( pxStreamBuffer )
#endif /* configINCLUDE_MESSAGE_BUFFER_AMP_DEMO */

extern void vAssertCalled( const char * const pcFileName,
                           unsigned long ulLine );

#ifndef projCOVERAGE_TEST
    #error projCOVERAGE_TEST should be defined to 1 or 0 on the command line.
#endif

#define configASSERT( x )    if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

#define configUSE_MALLOC_FAILED_HOOK    1

#define configMAC_ISR_SIMULATOR_PRIORITY    ( configMAX_PRIORITIES - 1 )

extern void vLoggingPrintf( const char * pcFormatString,
                            ... );

#define ipconfigHAS_DEBUG_PRINTF    1
#if ( ipconfigHAS_DEBUG_PRINTF == 1 )
    #define FreeRTOS_debug_printf( X )    vLoggingPrintf X
#endif

#define ipconfigHAS_PRINTF    0
#if ( ipconfigHAS_PRINTF == 1 )
    #define FreeRTOS_printf( X )    vLoggingPrintf X
#endif
#endif /* FREERTOS_CONFIG_H */
