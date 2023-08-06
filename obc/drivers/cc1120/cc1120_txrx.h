#pragma once

#include "obc_errors.h"

#include <stdint.h>

#include <FreeRTOS.h>
#include <os_semphr.h>

// See chapter 8.5 in the datasheet
#define TXRX_INTERRUPT_THRESHOLD 100U

typedef struct {
  uint8_t addr;
  uint8_t val;
} register_setting_t;

typedef enum {
  CC1120_STATE_SLEEP = 0,
  CC1120_STATE_IDLE,
  CC1120_STATE_XOFF,
  CC1120_STATE_BIAS_SETTLE_MC,
  CC1120_STATE_REG_SETTLE_MC,
  CC1120_STATE_MANCAL,
  CC1120_STATE_BIAS_SETTLE,
  CC1120_STATE_REG_SETTLE,
  CC1120_STATE_STARTCAL,
  CC1120_STATE_BWBOOST,
  CC1120_STATE_FS_LOCK,
  CC1120_STATE_IFADCON,
  CC1120_STATE_RX,
  CC1120_STATE_RX_END,
  CC1120_STATE_RESERVED,
  CC1120_STATE_TXRX_SWITCH,
  CC1120_STATE_RX_FIFO_ERR,
  CC1120_STATE_FSTXON,
  CC1120_STATE_TX,
  CC1120_STATE_TX_END,
  CC1120_STATE_RXTX_SWITCH,
  CC1120_STATE_TX_FIFO_ERR,
  CC1120_STATE_IFADCON_TXRX
} cc1120_state_t;

/**
 * @brief Initializes all of the semaphores that will be used by cc1120Send and cc1120Receive
 *
 */
void initAllTxRxSemaphores(void);

/**
 * @brief Gets the number of bytes queued in the TX FIFO
 *
 * @param numBytes - A pointer to an 8-bit integer to store the number of bytes in
 * @return obc_error_code_t - Whether or not the registe read was successful
 */
obc_error_code_t cc1120GetBytesInTxFifo(uint8_t *numBytes);

/**
 * @brief Gets the state of the CC1120 from the MARCSTATE register
 *
 * @param stateNum - A pointer to an 8-bit integer to store the state in
 * @return obc_error_code_t - Whether or not the register read was successful
 */
obc_error_code_t cc1120GetState(cc1120_state_t *stateNum);

/**
 * @brief Resets CC1120 & initializes transmit mode
 *
 * @return obc_error_code_t - Whether or not the setup was a success
 */
obc_error_code_t cc1120Init(void);

/**
 * @brief Adds the given data to the CC1120 FIFO buffer and transmits
 *
 * @param data - An array of 8-bit data to transmit
 * @param len - The size of the provided array
 * @param txFifoTimeout - The amount of time to wait for the txFifoEmptySemaphore to become available
 * @return obc_error_code_t
 */
obc_error_code_t cc1120Send(uint8_t *data, uint32_t len, TickType_t txFifoTimeout);

/* RX functions */
/**
 * @brief Gets the number of bytes queued in the RX FIFO
 *
 * @param numBytes - A pointer to an 8-bit integer to store the number of bytes in
 * @return obc_error_code_t - Whether or not the register read was successful
 */
obc_error_code_t cc1120GetBytesInRxFifo(uint8_t *numBytes);

/**
 * @brief Switches the cc1120 to RX mode to continuously receive bytes and send them to the decode task
 * @param syncTimeout - The amount of time to wait for the syncReceivedSemaphore to become available
 * @param rxTimeout - The amount of time to wait for the rxSemaphore to become available
 * @return obc_error_code_t
 */
obc_error_code_t cc1120Receive(TickType_t syncTimeout, TickType_t rxTimeout);

/**
 * @brief block until the tx fifo is empty without decrementing the semaphore
 * @param txFifoTimeout - The amount of time to wait for the txFifoEmptySemaphore to become available
 * @return obc_error_code_t - whether the tx fifo empty semaphore became available without timing out or not
 */
obc_error_code_t txFifoEmptyCheckBlocking(TickType_t txFifoTimeout);

/**
 * @brief callback function to be used in an ISR when the TX FIFO drops below (CC1120_TX_FIFO_SIZE -
 * TXRX_INTERRUPT_THRESHOLD)
 */
void txFifoReadyCallback(void);

/**
 * @brief callback function to be used in an ISR when the RX FIFO fills above TXRX_INTERRUPT_THRESHOLD
 */
void rxFifoReadyCallback(void);

/**
 * @brief callback function to be used in an ISR when the TX FIFO has become empty
 */
void txFifoEmptyCallback(void);

/**
 * @brief callback function to be used in an ISR when the sync word has been received
 */
void syncEventCallback(void);

/**
 * @brief allows other files to access the cc1120 RX semaphore handle
 *
 * @return SemaphoreHandle_t - handle of the cc1120 RX semaphore
 */
SemaphoreHandle_t getCC1120RxSemaphoreHandle(void);
