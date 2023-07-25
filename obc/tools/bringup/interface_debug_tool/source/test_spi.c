#include "test_spi.h"
#include "obc_sci_io.h"
#include "spi.h"
#include "reg_spi.h"
#include "spi.h"
#include <stdlib.h>

#define NUM_CHARS_TO_READ 1U
#define NUM_OUTBYTES 5U

void testSPI(void) {
  sciInit();
  spiInit();

  sciPrintf("Testing SPI...\r\n");
  sciPrintf("Which register module do you want to test?\n");

  uint16_t regNum;
  spiDAT1_t dataFmt = {.CS_HOLD = 0, .CSNR = SPI_CS_NONE, .DFSEL = SPI_FMT_0, .WDEL = 0};
  unsigned char buffer[NUM_CHARS_TO_READ] = {'\0'};
  char *c;

  while (1) {
    sciPrintf("Enter a valid register address\n");
    obc_error_code_t error = sciRead(buffer, NUM_CHARS_TO_READ);
    if (error == OBC_ERR_CODE_SUCCESS) {
      regNum = strtol(buffer, &c, 10);
      break;
    }
    sciPrintf("Error reading from SCI! - %d\r\n", (int)error);
  }

  uint32_t spiReg = spiREG1->GCR0 + regNum;
  uint8_t outBytes[NUM_OUTBYTES] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F};

  if (spiTransmitBytes(&spiReg, &dataFmt, outBytes, sizeof(outBytes) / sizeof(uint8_t)) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed transimitting data through SPI\r\n");
    return;
  }
  sciPrintf("Successfuly transimitted data through SPI\r\n");
}
