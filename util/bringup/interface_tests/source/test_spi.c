#include "test_spi.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"
#include "reg_spi.h"
#include "spi.h"

void testSPI(void) {
    sciPrintf("Testing SPI...\r\n");
    spiBASE_t spiReg;
    spiDAT1_t spiDataFormat;
    uint8_t *outBytes = {0x34, 0x00, 0x77, 0xFF, 0xA3};
    size_t numBytes = 5;
    
    if(spiTransmitBytes(&spiReg, &spiDataFormat, outBytes, numBytes)!=
        OBC_ERR_CODE_SUCCESS){
            sciPrintf("Failed transimitting data through SPI\r\n");
            return;
        }
    sciPrintf("Successfuly transimitted data through SPI\r\n");
}
