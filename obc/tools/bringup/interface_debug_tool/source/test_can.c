#include "test_can.h"
#include "obc_sci_io.h"
#include "can.h"

#define REG_NUM_CHARS 1U

// CAN Message Box Number Rule:
//	TX: 1
//	RX: 2
#define TXMessageBox 1

void testCAN(void) {
  sciPrintf("Testing CAN...\r\n");
  canInit();

  uint16_t regNum;
  unsigned char buffer[REG_NUM_CHARS] = {'\0'};
  char *c;

  while (1) {
    sciPrintf("Enter a valid register address\n");
    obc_error_code_t error = sciRead(buffer, REG_NUM_CHARS);
    if (error == OBC_ERR_CODE_SUCCESS) {
      regNum = strtol(buffer, &c, 10);
      break;
    }
    sciPrintf("Error reading from SCI! - %d\r\n", (int)error);
  }

  uint32 canReg = canREG1->CTL + regNum;
  const uint8 txData = 0xFFU;
  if (canTransmit(&canReg, TXMessageBox, &txData) == 0U) {
    sciPrintf("Tramsmission Failed.\r\n");
    return;
  }
  sciPrintf("Transmission Succeeded.\r\n");
}
