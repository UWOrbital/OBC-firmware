#include "test_adc.h"
#include "obc_sci_io.h"
#include "adcs_manager.h"

void testADC(void) {
    sciPrintf("Testing ADC...\r\n");

    initADCSManager();
    adcs_event_t adcEvent;
    if (sendToADCSQueue(adcEvent) != OBC_ERR_CODE_SUCCESS)
    {
        sciPrintf("Failed sending event through ADC\r\n");
        return;
    }
    sciPrintf("Successfully sending event through ADC \r\n");
}