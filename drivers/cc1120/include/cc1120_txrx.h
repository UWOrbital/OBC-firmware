#ifndef DRIVERS_CC1120_INCLUDE_CC1120_TXRX_H
#define DRIVERS_CC1120_INCLUDE_CC1120_TXRX_H

#include <stdint.h>
#include "obc_logging.h"
#include "obc_math.h"
#include "ax25.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <FreeRTOSConfig.h>

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
 * @return obc_error_code_t
 */
obc_error_code_t cc1120Send(uint8_t *data, uint32_t len);

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
 *
 * @return obc_error_code_t
 */
obc_error_code_t cc1120Receive(void);

/**
 * @brief block until the tx fifo is empty without decrementing the semaphore
 *
 * @return obc_error_code_t - whether the tx fifo empty semaphore became available without timing out or not
 */
obc_error_code_t txFifoEmptyCheckBlocking(void);

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

#endif /* DRIVERS_CC1120_INCLUDE_CC1120_TXRX_H */
