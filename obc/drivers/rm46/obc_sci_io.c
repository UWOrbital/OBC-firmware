#include "obc_sci_io.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sci.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static SemaphoreHandle_t sciMutex = NULL;
static StaticSemaphore_t sciMutexBuffer;
static SemaphoreHandle_t sciLinMutex = NULL;
static StaticSemaphore_t sciLinMutexBuffer;

// Semaphore to signal when an async transfer is complete
static SemaphoreHandle_t sciTransferComplete = NULL;
static StaticSemaphore_t sciTransferCompleteBuffer;

static uint8_t *sciRxBuff = NULL;
static size_t sciRxBuffLen = 0;

STATIC_ASSERT((UART_PRINT_REG == sciREG) || (UART_PRINT_REG == scilinREG),
              "UART_PRINT_REG must be sciREG or scilinREG");
STATIC_ASSERT((UART_READ_REG == sciREG) || (UART_READ_REG == scilinREG), "UART_READ_REG must be sciREG or scilinREG");

void initSciMutex(void) {
  if (sciMutex == NULL) {
    sciMutex = xSemaphoreCreateMutexStatic(&sciMutexBuffer);
  }
  configASSERT(sciMutex);

  if (sciLinMutex == NULL) {
    sciLinMutex = xSemaphoreCreateMutexStatic(&sciLinMutexBuffer);
  }
  configASSERT(sciLinMutex);

  if (sciTransferComplete == NULL) {
    sciTransferComplete = xSemaphoreCreateBinaryStatic(&sciTransferCompleteBuffer);
  }
  configASSERT(sciTransferComplete);

  sciSetBaudrate(UART_READ_REG, OBC_UART_BAUD_RATE);
}

/*
obc_error_code_t sciReadByte(unsigned char *character) {
    SemaphoreHandle_t mutex = (UART_READ_REG == sciREG) ? sciMutex : sciLinMutex;
    configASSERT(mutex != NULL);

    if (character == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xSemaphoreTake(mutex, UART_MUTEX_BLOCK_TIME) == pdTRUE) {
        *character = (unsigned char)sciReceiveByte(UART_READ_REG); // sciReceiveByte applies a 0xFF mask
        xSemaphoreGive(mutex);
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}
*/

obc_error_code_t sciReadBytes(uint8_t *buf, size_t numBytes, TickType_t uartMutexTimeoutTicks, size_t blockTimeTicks,
                              sciBASE_t *sciReg) {
  obc_error_code_t errCode;

  if (!(sciReg == scilinREG || sciReg == sciREG)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  SemaphoreHandle_t mutex = (sciReg == UART_READ_REG) ? sciMutex : sciLinMutex;
  configASSERT(mutex != NULL);

  if (buf == NULL || numBytes < 1) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xSemaphoreTake(mutex, uartMutexTimeoutTicks) != pdTRUE) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  sciRxBuff = buf;
  sciRxBuffLen = numBytes;

  // Start asynchronous transfer
  sciReceive(sciReg, numBytes, buf);

  // Wait for transfer to complete
  if (xSemaphoreTake(sciTransferComplete, blockTimeTicks) != pdTRUE) {
    errCode = OBC_ERR_CODE_SEMAPHORE_TIMEOUT;
  } else {
    errCode = OBC_ERR_CODE_SUCCESS;
  }

  sciRxBuff = NULL;
  sciRxBuffLen = 0;

  xSemaphoreGive(mutex);
  return errCode;
}

obc_error_code_t sciSendBytes(uint8_t *buf, size_t numBytes, TickType_t uartMutexTimeoutTicks, sciBASE_t *sciReg) {
  if (!(sciReg == scilinREG || sciReg == sciREG)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  SemaphoreHandle_t mutex = (sciReg == UART_PRINT_REG) ? sciMutex : sciLinMutex;
  configASSERT(mutex != NULL);

  if (buf == NULL || numBytes < 1) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xSemaphoreTake(mutex, uartMutexTimeoutTicks) != pdTRUE) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  sciSend(sciReg, numBytes, buf);

  xSemaphoreGive(mutex);
  return OBC_ERR_CODE_SUCCESS;
}

/*
obc_error_code_t sciRead(unsigned char *text, uint32_t length) {
    SemaphoreHandle_t mutex = (UART_READ_REG == sciREG) ? sciMutex : sciLinMutex;
    configASSERT(mutex != NULL);

    if (text == NULL || length < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint32_t actualLength = 0;
    unsigned char cChar;

    if (xSemaphoreTake(mutex, UART_MUTEX_BLOCK_TIME) == pdTRUE) {
        while(1) {
            cChar = (unsigned char) sciReceiveByte(UART_READ_REG); // sciReceiveByte applies a 0xFF mask

            if (cChar == '\b') {
                if(actualLength > 0) {
                    text[actualLength - 1] = '\0';
                    actualLength--;
                }
                continue;
            }

            if ((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
                break;

            text[actualLength] = cChar;
            actualLength++;

            if (actualLength == (length - 1))
                break;

        }
        text[actualLength] = '\0';
        xSemaphoreGive(mutex);
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}
*/

void sciNotification(sciBASE_t *sci, uint32 flags) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (!(sci == scilinREG || sci == sciREG)) {
    return;
  }

  if (flags == SCI_RX_INT) {
    sciReceive(sci, sciRxBuffLen, sciRxBuff);
    xSemaphoreGiveFromISR(sciTransferComplete, &xHigherPriorityTaskWoken);
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
