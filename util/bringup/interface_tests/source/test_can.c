#include "test_can.h"
#include "obc_sci_io.h"
#include "can.h"

// A number of Mailbox in CAN bus is in between 1 and 64
#define boxNum 1

void testCAN(void) {
  sciPrintf("Testing CAN...\r\n");
  canInit();
  const uint8 txData = 0xFFU;
  if (canTransmit(canREG1, boxNum, &txData) == 0U) {
    sciPrintf("Tramsmission Failed.\r\n");
    return;
  }

  sciPrintf("Transmission Succeeded.\r\n");
}
