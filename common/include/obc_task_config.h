//include macros for task name, priority, stack size, etc.

/* Supervisor task config */
#define SUPERVISOR_STACK_SIZE   1024U
#define SUPERVISOR_NAME         "supervisor"
#define SUPERVISOR_PRIORITY     tskIDLE_PRIORITY + 1

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE   1024U
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY      tskIDLE_PRIORITY + 1

/* ADCS Manager task config */
#define ADCS_MANAGER_STACK_SIZE   1024U
#define ADCS_MANAGER_NAME         "adcs_manager"
#define ADCS_MANAGER_PRIORITY       tskIDLE_PRIORITY + 1

/* Comms Manager task config */
#define COMMS_MANAGER_STACK_SIZE   1024U
#define COMMS_MANAGER_NAME         "comms_manager"
#define COMMS_MANAGER_PRIORITY      tskIDLE_PRIORITY + 1

/* EPS Manager task config */
#define EPS_MANAGER_STACK_SIZE   1024U
#define EPS_MANAGER_NAME         "eps_manager"
#define EPS_MANAGER_PRIORITY     tskIDLE_PRIORITY + 1

/* Payload Manager task config */
#define PAYLOAD_MANAGER_STACK_SIZE   1024U
#define PAYLOAD_MANAGER_NAME         "payload_manager"
#define PAYLOAD_MANAGER_PRIORITY     tskIDLE_PRIORITY + 1
