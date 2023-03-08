#ifndef CC1120_TXRX_H
#define CC1120_TXRX_H

#include <stdint.h>
#include "cc1120_regs.h"
#include "obc_logging.h"

#define CC1120_MAX_PACKET_LEN 255
#define CC1120_TX_FIFO_SIZE 128

#define min(a, b) (a < b ? a : b)

typedef struct
{
    uint8_t addr;
    uint8_t val;
} registerSetting_t;

void initRxSemaphore(void)

void initTxSemaphore(void);
/**
 * @brief Gets the number of packets queued in the TX FIFO
 *
 * @param numPackets - A pointer to an 8-bit integer to store the number of packets in
 * @return obc_error_code_t - Whether or not the registe read was successful
 */
obc_error_code_t cc1120_get_packets_in_tx_fifo(uint8_t *numPackets);

/**
 * @brief Gets the state of the CC1120 from the MARCSTATE register
 *
 * @param stateNum - A pointer to an 8-bit integer to store the state in
 * @return obc_error_code_t - Whether or not the register read was successful
 */
obc_error_code_t cc1120_get_state(uint8_t *stateNum);

/**
 * @brief Resets CC1120 & initializes transmit mode
 *
 * @return obc_error_code_t - Whether or not the setup was a success
 */
obc_error_code_t cc1120_init();

/**
 * @brief Adds the given data to the CC1120 FIFO buffer and transmits
 *
 * @param data - An array of 8-bit data to transmit
 * @param len - The size of the provided array
 * @return obc_error_code_t
 */
obc_error_code_t cc1120_send(uint8_t *data, uint32_t len);

/* RX functions */
/**
 * @brief Gets the number of packets queued in the TX FIFO
 *
 * @param numPackets - A pointer to an 8-bit integer to store the number of packets in
 * @return obc_error_code_t - Whether or not the register read was successful
 */
obc_error_code_t cc1120_get_packets_in_rx_fifo(uint8_t *numPackets);

/**
 * @brief Switches the cc1120 to RX mode to receive 278 bytes
 *
 * @param data - an array of 8-bit data with size of atleast 278 where received data is stored
 * @return obc_error_code_t
 */
obc_error_code_t cc1120_receive(uint8_t data[]);

/**
 * @brief Gets the handle of the RX semaphore
 *
 * @return SemaphoreHandle_t - The handle of the RX Semaphore
 */
SemaphoreHandle_t getRxSemaphore();

/**
 * @brief Gets the handle of the TX semaphore
 *
 * @return SemaphoreHandle_t - The handle of the TX Semaphore
 */
SemaphoreHandle_t getTxSemaphore();

#endif /* CC1120_TXRX_H */
