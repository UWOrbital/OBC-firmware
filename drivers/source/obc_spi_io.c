#include "obc_spi_io.h"
#include "obc_errors.h"
#include "logging.h"

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

/**
 * @brief Check if a SPI CS pin is valid.
 * 
 * @param spiPort The SPI port to use.
 * @param csNum The chip select pin to use.
 * @return true if the CS pin is valid; false otherwise.
 */
static bool isValidCSNum(gioPORT_t *spiPort, uint8_t csNum);

static SemaphoreHandle_t spiMutexes[NUM_SPI_PORTS];
static StaticSemaphore_t spiMutexBuffers[NUM_SPI_PORTS];
static const uint8_t numCSPins[NUM_SPI_PORTS] = { 6, 0, 6, 1, 4 }; // Number of chip select pins for each SPI port

void initSpiMutex(void) {
    for (int i = 0; i < NUM_SPI_PORTS; i++) {
        spiMutexes[i] = xSemaphoreCreateMutexStatic(&spiMutexBuffers[i]);
        configASSERT(spiMutexes[i]);
    }
}

obc_error_code_t deassertChipSelect(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (!isValidCSNum(spiPort, csNum))
        return OBC_ERR_CODE_INVALID_ARG;

    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xSemaphoreTake(spiMutexes[spiPortIndex], portMAX_DELAY) == pdTRUE) {
        spiPort->DSET = (1 << csNum);
        xSemaphoreGive(spiMutexes[spiPortIndex]); // Can only fail if the mutex wasn't taken; we just took it, so this will never fail
        return OBC_ERR_CODE_SUCCESS;
    }
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t assertChipSelect(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (!isValidCSNum(spiPort, csNum))
        return OBC_ERR_CODE_INVALID_ARG;

    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xSemaphoreTake(spiMutexes[spiPortIndex], portMAX_DELAY) == pdTRUE) {
        spiPort->DCLR = (1 << csNum);
        xSemaphoreGive(spiMutexes[spiPortIndex]); // Can only fail if the mutex wasn't taken; we just took it, so this will never fail
        return OBC_ERR_CODE_SUCCESS;
    }
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t spiTransmitAndReceiveByte(spiBASE_t *spiReg, uint8_t outb, uint8_t *inb) {
    if (spiReg == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (inb == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    int8_t spiRegIndex = spiRegToIndex(spiReg);
    if (spiRegIndex < 0)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xSemaphoreTake(spiMutexes[spiRegIndex], portMAX_DELAY) == pdTRUE) {        
        spiDAT1_t spiData = {0};

        // The SPI HAL functions take 16-bit arguments, but we're using 8-bit word size
        uint16_t spiWordOut = (uint16_t)outb;
        uint16_t spiWordIn;

        uint32_t spiErr = spiTransmitAndReceiveData(spiReg, &spiData, 1, &spiWordOut, &spiWordIn) & SPI_FLAG_ERR_MASK;
        obc_error_code_t ret;

        if (spiErr != SPI_FLAG_SUCCESS) {
            // To-do: Log and handle errors
            ret = OBC_ERR_CODE_SPI_FAILURE;
        } else {
            *inb = (uint8_t)spiWordIn;
            ret = OBC_ERR_CODE_SUCCESS;
        }

        xSemaphoreGive(spiMutexes[spiRegIndex]);
        return ret;
    }
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t spiTransmitByte(spiBASE_t *spiReg, uint8_t outb) {
    obc_error_code_t errCode;
    uint8_t inb;

    RETURN_IF_ERROR_CODE(spiTransmitAndReceiveByte(spiReg, outb, &inb));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t spiReceiveByte(spiBASE_t *spiReg, uint8_t *inb) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(spiTransmitAndReceiveByte(spiReg, 0xFF, inb));
    return OBC_ERR_CODE_SUCCESS;
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

static bool isValidCSNum(gioPORT_t *spiPort, uint8_t csNum) {
    if (spiPort == NULL)
        return false;

    int8_t spiPortIndex = spiPortToIndex(spiPort);
    if (spiPortIndex < 0)
        return false;

    // Each SPI port's CS pins are numbered from 0 to numCSPins[spiPortIndex] - 1
    if (csNum < numCSPins[spiPortIndex])
        return true;

    return false;
}