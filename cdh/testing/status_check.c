#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t* start_A_collection();

void status_check_main() {
    TaskHandle_t *Acollector = start_A_collection();        
}
