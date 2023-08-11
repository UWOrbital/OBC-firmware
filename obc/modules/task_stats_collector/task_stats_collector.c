#if (DEBUG == 1)
#include "task_stats_collector.h"
#include "obc_task_config.h"
#include "obc_sci_io.h"
#include "obc_privilege.h"

#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>

#define TASK_STATS_BUFFER_SIZE 1000U
static StackType_t taskStatsCollectorStack[TASK_STATS_COLLECTOR_STACK_SIZE];
static StaticTask_t taskStatsCollectorTaskBuffer;
static TaskHandle_t taskStatsCollectorTaskHandle;

static char taskTableHeaderStr[] =
    "***********************************************\r\nTask           State   Prio    Stack    "
    "Num\r\n***********************************************\r\n";

static void vTaskStatsCollector(void *pvParameters);
/**
 * @brief Initialize the task stats collector for debug
 */
void initTaskStatsCollector(void) {
  ASSERT((taskStatsCollectorStack != NULL) && (&taskStatsCollectorTaskBuffer != NULL));
  taskStatsCollectorTaskHandle =
      xTaskCreateStatic(vTaskStatsCollector, TASK_STATS_COLLECTOR_NAME, TASK_STATS_COLLECTOR_STACK_SIZE, NULL,
                        TASK_STATS_COLLECTOR_PRIORITY, taskStatsCollectorStack, &taskStatsCollectorTaskBuffer);
}

static void vTaskStatsCollector(void *pvParameters) {
  prvRaisePrivilege();
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(15000));
    char taskStatsString[TASK_STATS_BUFFER_SIZE] = {0};
    vTaskList(taskStatsString);
    sciPrintText((unsigned char *)taskTableHeaderStr, strlen(taskTableHeaderStr));
    sciPrintText((unsigned char *)taskStatsString, TASK_STATS_BUFFER_SIZE);
  }
}
#endif
