#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"

#include "cc1120_spi_tests.h"
#include "cc1120_spi.h"
#include "rs_test.h"

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_portable.h"

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <spi.h>


static TaskHandle_t testTaskHandle = NULL;
static StaticTask_t testTaskBuffer;
static StackType_t testTaskStack[1024U];

#include "cc1120_defs.h"
#include "cc1120_txrx.h"
void transmitTest() {
    uint8_t data[6] = {0x05, 0x00, 0x01, 0x02, 0x03, 0x04};
    cc1120StrobeSpi(CC1120_STROBE_SFTX);
    cc1120Send(data, 6);
}

void initTestTask(void);
static void vTestTask(void * pvParameters);

void initTestTask(void) {
    ASSERT( (testTaskStack != NULL) && (&testTaskBuffer != NULL) );
    if (testTaskHandle == NULL) {
        testTaskHandle = xTaskCreateStatic(vTestTask, "test task", 1024U, NULL, 1U, testTaskStack, &testTaskBuffer);
    }
}

static void vTestTask(void * pvParameters) {
    //Run the E2E SPI read test
    // cc1120TestSpiRead();
    // testRs();
    cc1120Init();
    LOG_ERROR_CODE(5);
    uint8_t data = 0x4F;
    cc1120WriteSpi(CC1120_REGS_PA_CFG2, &data, 1);
    cc1120StrobeSpi(CC1120_STROBE_STX);
    while(1) {

    }
}

int main(void) {
    gioInit();
    sciInit();
    spiInit();

    // Initialize logger
    initLogger();
    logSetLevel(LOG_DEBUG);
    
    // Initialize bus mutexes
    initSciMutex();
    initSpiMutex();

    initTestTask();

    vTaskStartScheduler();
}
