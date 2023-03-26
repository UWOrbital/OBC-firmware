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

#define MAX_PRINTF_SIZE 128U
#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static SemaphoreHandle_t sciMutex = NULL;
static StaticSemaphore_t sciMutexBuffer;
static SemaphoreHandle_t sciLinMutex = NULL;
static StaticSemaphore_t sciLinMutexBuffer;

STATIC_ASSERT((UART_PRINT_REG == sciREG) || (UART_PRINT_REG == scilinREG), "UART_PRINT_REG must be sciREG or scilinREG");
STATIC_ASSERT((UART_READ_REG == sciREG) || (UART_READ_REG == scilinREG), "UART_READ_REG must be sciREG or scilinREG");

/**
 * @brief Iterate through an array of bytes and transmit them via UART_PRINT_REG.
 * 
 * @param bytes The array of bytes to transmit.
 * @param length The length of the array of bytes to transmit.
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
static obc_error_code_t sciSendBytes(unsigned char *bytes, uint32_t length);

void initSciMutex(void) {
    if (sciMutex == NULL) {
        sciMutex = xSemaphoreCreateMutexStatic(&sciMutexBuffer);
    }
    if (sciLinMutex == NULL) {
        sciLinMutex = xSemaphoreCreateMutexStatic(&sciLinMutexBuffer);
    }

    configASSERT(sciMutex);
    configASSERT(sciLinMutex);
}

obc_error_code_t sciPrintText(unsigned char *text, uint32_t length) {
    if (text == NULL || length == 0)
        return OBC_ERR_CODE_INVALID_ARG;

    SemaphoreHandle_t mutex = (UART_PRINT_REG == sciREG) ? sciMutex : sciLinMutex;
    configASSERT(mutex);

    if (xSemaphoreTake(mutex, UART_MUTEX_BLOCK_TIME) == pdTRUE) {
        obc_error_code_t err = sciSendBytes(text, length);
        xSemaphoreGive(mutex);
        return err;
    }
    
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

static obc_error_code_t sciSendBytes(unsigned char *bytes, uint32_t length) {
    if (bytes == NULL || length == 0U)
        return OBC_ERR_CODE_INVALID_ARG;
    
    for (uint32_t i = 0; i < length; i++) {
        if (bytes[i] == '\0')
            break;
        // sciSendByte waits for the transmit buffer to be empty before sending
        sciSendByte(UART_PRINT_REG, bytes[i]);
    }
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sciPrintf(const char *s, ...){
    if (s == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    char buf[MAX_PRINTF_SIZE] = {0};
    
    va_list args;
    va_start(args, s);
    int n = vsnprintf(buf, MAX_PRINTF_SIZE, s, args);
    va_end(args);

    if (n < 0)
        return OBC_ERR_CODE_INVALID_ARG;

    // n == MAX_PRINTF_SIZE invalid because null character isn't included in count
    if ((uint32_t)n >= MAX_PRINTF_SIZE)
        return OBC_ERR_CODE_INVALID_ARG;

    return sciPrintText((unsigned char *)buf, MAX_PRINTF_SIZE);
}

void uartAssertFailed(char *file, int line, char *expr) {
    if (file == NULL || line < 0 || expr == NULL)
        return; // Only called by assert, so we can assume that the arguments are valid
    
    sciPrintf("ASSERTION FAILED: %s, file %s, line %d\r\n",
                            expr, file, line);
}

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