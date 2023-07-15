#include "test_can.h"
#include "obc_sci_io.h"
#include "can.h"

void testCAN(void) {
  sciPrintf("Testing CAN...\r\n");
  canInit();
  const uint8 txData = 0xFFU;
  if (canTransmit(canREG1, 10, &txData) == 0U) {
    sciPrintf("Tramsmission Failed.\r\n");
  }

  sciPrintf("Transmission Succeeded.\r\n");
}
