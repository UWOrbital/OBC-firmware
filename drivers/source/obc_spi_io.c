#include "obc_spi_io.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>

#include <gio.h>
#include <spi.h>

/**
 * @brief Get index of spiPort in spiMutexes and spiMutexBuffers
 * 
 * @param spiPort The SPI port to use.
 * @return int8_t The index of the SPI port in spiMutexes and spiMtexBuffers; -1 if not found.
 */
static int8_t spiPortToIndex(gioPORT_t *spiPort);

/**
 * @brief Get index of spiReg in spiMutexes and spiMutexBuffers
 * 
 * @param spiReg The SPI register to use. 
 * @return int8_t The index of the SPI register in spiMutexes and spiMtexBuffers; -1 if not found.
 */
static int8_t spiRegToIndex(spiBASE_t *spiReg);

static SemaphoreHandle_t spiMutexes[NUM_SPI_PORTS];
static StaticSemaphore_t spiMutexBuffers[NUM_SPI_PORTS];

void initSpiMutex(void) {
    for (int i = 0; i < NUM_SPI_PORTS; i++) {
        spiMutexes[i] = xSemaphoreCreateMutexStatic(&spiMutexBuffers[i]);
        configASSERT(spiMutexes[i]);
    }
}

obc_error_code_t deassertChipSelect(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // TODO: Check valid CS number
    
    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xSemaphoreTake(spiMutexes[spiPortIndex], portMAX_DELAY) == pdTRUE) {
        spiPort->DSET = (1 << csNum);
        xSemaphoreGive(spiMutexes[spiPortIndex]); // Can only fail if the mutex wasn't taken
        return OBC_ERR_CODE_SUCCESS;
    }
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t assertChipSelect(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // TODO: Check valid CS number

    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (xSemaphoreTake(spiMutexes[spiPortIndex], portMAX_DELAY) == pdTRUE) {
        spiPort->DCLR = (1 << csNum);
        xSemaphoreGive(spiMutexes[spiPortIndex]); // Can only fail if the mutex wasn't taken
        return OBC_ERR_CODE_SUCCESS;
    }
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t spiTransmitAndReceiveByte(spiBASE_t *spiReg, unsigned char outb, unsigned char *inb) {
    if (spiReg == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (inb == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    int8_t spiRegIndex = spiRegToIndex(spiReg);
    if (spiRegIndex < 0)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (xSemaphoreTake(spiMutexes[spiRegIndex], portMAX_DELAY) == pdTRUE) {        
        spiDAT1_t spiData = {0};
        uint16_t spiWordOut = (uint16_t)outb;
        uint16_t spiWordIn;
        
        spiTransmitAndReceiveData(spiReg, &spiData, 1, &spiWordOut, &spiWordIn); // TODO: Check return value
        *inb = (unsigned char)spiWordIn;
        
        xSemaphoreGive(spiMutexes[spiRegIndex]); // Can only fail if the mutex wasn't taken
        return OBC_ERR_CODE_SUCCESS;
    }
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t spiTransmitByte(spiBASE_t *spiReg, unsigned char outb) {
    unsigned char inb;
    return spiTransmitAndReceiveByte(spiReg, outb, &inb);
}

obc_error_code_t spiReceiveByte(spiBASE_t *spiReg, unsigned char *inb) {
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