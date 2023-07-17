#include "test_can.h"
#include "obc_sci_io.h"
#include "can.h"

// CAN Message Box Number Rule:
//	TX: 1
//	RX: 2
#define TXMessageBox 1

void testCAN(void) {
  sciPrintf("Testing CAN...\r\n");
  canInit();
  const uint8 txData = 0xFFU;
  if (canTransmit(canREG1, TXMessageBox, &txData) == 0U) {
    sciPrintf("Tramsmission Failed.\r\n");
    return;
  }

  sciPrintf("Transmission Succeeded.\r\n");
}
