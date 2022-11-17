#include "obc_spi_io.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>

#include <gio.h>
#include <spi.h>

static int8_t spiPortToIndex(gioPORT_t *spiPort);
static int8_t spiRegToIndex(spiBASE_t *spiReg);

static SemaphoreHandle_t spiMutexes[NUM_SPI_PORTS];
static StaticSemaphore_t spiMutexBuffers[NUM_SPI_PORTS];

void initSpiMutex(void) {
    for (int i = 0; i < NUM_SPI_PORTS; i++) {
        spiMutexes[i] = xSemaphoreCreateMutexStatic(&spiMutexBuffers[i]);
        configASSERT(spiMutexes[i]);
    }
}

/**
 * @brief Deselect the SD card's SPI chip select.
 */
uint8_t deassertChipSelect(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return 0;

    // TODO: Check valid CS number
    
    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return 0;

    if (xSemaphoreTake(spiMutexes[spiPortIndex], portMAX_DELAY) == pdTRUE) {
        spiPort->DSET = (1 << csNum);
        xSemaphoreGive(spiMutexes[spiPortIndex]);
    }
    return 1;
}

/**
 * @brief Select the SD card's SPI chip select.
 */
uint8_t assertChipSelect(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return 0;

    // TODO: Check valid CS number

    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return 0;
    
    if (xSemaphoreTake(spiMutexes[spiPortIndex], portMAX_DELAY) == pdTRUE) {
        spiPort->DCLR = (1 << csNum);
        xSemaphoreGive(spiMutexes[spiPortIndex]);
    }
    return 1;
}

/**
 * @brief Read and write a byte to the SD card's SPI interface.
*/
uint8_t spiTransmitAndReceiveByte(spiBASE_t *spiReg, unsigned char outb, unsigned char *inb) {
    if (spiReg == NULL)
        return 0;
    
    if (inb == NULL)
        return 0;

    int8_t spiRegIndex = spiRegToIndex(spiReg);
    if (spiRegIndex < 0)
        return 0;
    
    if (xSemaphoreTake(spiMutexes[spiRegIndex], portMAX_DELAY) == pdTRUE) {
        // while ((spiReg->FLG & 0x0200) == 0); // Wait until TXINTFLG is set for previous transmission
        // spiReg->DAT1 = outb | 0x100D0000;    // transmit register address

        // while ((spiReg->FLG & 0x0100) == 0); // Wait until RXINTFLG is set when new value is received
        // *inb = (unsigned char)spiReg->BUF;                 // read received value
        
        spiDAT1_t spiData = {0};
        uint16_t spiWordOut = (uint16_t)outb;
        uint16_t spiWordIn;
        spiTransmitAndReceiveData(spiReg, &spiData, 1, &spiWordOut, &spiWordIn);
        *inb = (unsigned char)spiWordIn;
        
        xSemaphoreGive(spiMutexes[spiRegIndex]);
        return 1;
    }
    return 0;
}

/**
 * @brief Transmit a byte to the SD card's SPI interface.
 * @param dat Byte to transmit.
 */
uint8_t spiTransmitByte(spiBASE_t *spiReg, unsigned char outb) {
    if (spiReg == NULL)
        return 0;

    int8_t spiRegIndex = spiRegToIndex(spiReg);
    if (spiRegIndex < 0)
        return 0;
    
    // while ((spiReg->FLG & 0x0200U) == 0); // Wait until TXINTFLG is set for previous transmission
    // spiReg->DAT1 = (uint32_t)outb;    // transmit register address

    // while ((spiReg->FLG & 0x0100U) == 0); // Wait until RXINTFLG is set when new value is received
    // unsigned int ui32RcvDat;
    // ui32RcvDat = spiReg->BUF;  // to get received value
    // ui32RcvDat = ui32RcvDat;        // to avoid compiler warning
    
    unsigned char inb;
    return spiTransmitAndReceiveByte(spiReg, outb, &inb);
}

/**
 * @brief Receive a byte from the SD card's SPI interface.
 * 
 * @return BYTE Received byte.
 */
unsigned char spiReceiveByte(spiBASE_t *spiReg, unsigned char *inb) {
    if (spiReg == NULL)
        return 0;
    
    if (inb == NULL)
        return 0;

    return spiTransmitAndReceiveByte(spiReg, 0xFF, inb);
}

static int8_t spiPortToIndex(gioPORT_t *spiPort) {
    if (spiPort == spiPORT1)
        return 0;
    if (spiPort == spiPORT2)
        return 1;
    if (spiPort == spiPORT3)
        return 2;
    if (spiPort == spiPORT4)
        return 3;
    if (spiPort == spiPORT5)
        return 4;
    return -1;
}

static int8_t spiRegToIndex(spiBASE_t *spiReg) {
    if (spiReg == spiREG1)
        return 0;
    if (spiReg == spiREG2)
        return 1;
    if (spiReg == spiREG3)
        return 2;
    if (spiReg == spiREG4)
        return 3;
    if (spiReg == spiREG5)
        return 4;
    return -1;
}