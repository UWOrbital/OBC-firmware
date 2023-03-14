//include macros for task name, priority, stack size, etc.

/* Supervisor task config */
#define SUPERVISOR_STACK_SIZE   1024U
#define SUPERVISOR_NAME         "supervisor"
#define SUPERVISOR_PRIORITY     1U
#define SUPERVISOR_DELAY_TICKS  pdMS_TO_TICKS(1000)

/* Supervisor queue config */
#define SUPERVISOR_QUEUE_LENGTH 10U
#define SUPERVISOR_QUEUE_ITEM_SIZE sizeof(supervisor_event_t)
#define SUPERVISOR_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define SUPERVISOR_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE   1024U
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY     1U
#define TELEMETRY_DELAY_TICKS  pdMS_TO_TICKS(1000)

/* Telemetry queue config */
#define TELEMETRY_QUEUE_LENGTH 10U
#define TELEMETRY_QUEUE_ITEM_SIZE sizeof(telemetry_event_t)
#define TELEMETRY_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define TELEMETRY_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)