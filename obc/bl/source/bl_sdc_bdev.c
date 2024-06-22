#include "bl_config.h"
#include "bl_flash.h"
#include "bl_uart.h"
#include "obc_board_config.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <spi.h>

/*---------------------------------------------*/
/* SPI HAL Definitions                         */
/*---------------------------------------------*/
// SPIFLG Errors
#define SPI_FLAG_ERR_MASK 0xFFU   // All errors are shown in the lowest byte of SPIFLG
#define SPI_FLAG_SUCCESS 0x00U    // No errors
#define SPI_FLAG_DLENERR 0x01U    // Data length error
#define SPI_FLAG_TIMEOUT 0x02U    // Timeout error
#define SPI_FLAG_PARERR 0x04U     // Parity error
#define SPI_FLAG_DESYNC 0x08U     // Desynchronization error
#define SPI_FLAG_BITERR 0x10U     // Bit error
#define SPI_FLAG_RXOVRNINT 0x40U  // Receive overrun interrupt flag

/*---------------------------------------------*/
/* SD Card Definitions                         */
/*---------------------------------------------*/
#define SDC_ACTION_NUM_ATTEMPTS_DEFAULT 50U
#define SDC_MOSI_HIGH 0xFFU             // Keep MOSI high during read operations
#define SDC_CLOCK_TRANSITION_BYTES 10U  // 10 bytes = 80 clock transitions
#define SDC_CMD17_DATA_TOKEN 0xFEU

/*---------------------------------------------*/
/* Global Variables                            */
/*---------------------------------------------*/
// SPI configuration for SD card
// CS selected using SPI assert/deassert functions; not using CSNR
static spiDAT1_t sdcSpiConfig = {
    .CS_HOLD = false,
    .WDEL = false,
    .DFSEL = SDC_SPI_DATA_FORMAT,
    .CSNR = SPI_CS_NONE,
};

typedef enum {
  POWER_OFF,
  POWER_ON,
} sdc_power_t;

static sdc_power_t powerFlag = POWER_OFF; /* indicates if "power" is on */

/*---------------------------------------------*/
/* SD Card Private Functions                   */
/*---------------------------------------------*/

/**
 * @brief Check if card is ready. NOTE: CS must be asserted prior to calling this.
 *
 * @return bool True if card is ready, false otherwise.
 */
static bool blIsCardReady(void) {
  // Assume CS is already asserted
  for (uint8_t i = 0; i < SDC_ACTION_NUM_ATTEMPTS_DEFAULT; i++) {
    uint16_t res = 0;
    spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, &res);

    if (res == 0xFF) return true;
  }

  return false;
}

/**
 * @brief Send >74 clock transitions with CS and DI held high. This is
 * required after card power up to get it into SPI mode.
 */
static void sendClockTrain(void) {
  // Assume CS is not asserted
  for (uint8_t i = 0; i < SDC_CLOCK_TRANSITION_BYTES; i++) {
    uint16_t out = 1;
    spiTransmitData(SDC_SPI_REG, &sdcSpiConfig, 1, &out);
  }
}

/**
 * @brief Turn the SD card power on.
 * @warning This function doesn't actually turn the power on, it just
 * sets a flag to indicate that the power is on.
 */
static void turnOnSDC(void) {
  sendClockTrain();
  powerFlag = POWER_ON;
}

/**
 * @brief Turn the SD card power off.
 * @warning This function doesn't actually turn the power off, it just
 * sets a flag to indicate that the power is off.
 */
static void turnOffSDC(void) { powerFlag = POWER_OFF; }

/**
 * @brief Check the power status of the SD card.
 *
 * @return sdc_power_t POWER_ON if the SD card is powered on, POWER_OFF otherwise.
 */
static sdc_power_t checkPower(void) { return powerFlag; }

/**
 * @brief Receive a data packet from the SD card.
 * @param buff Buffer to store the received data.
 * @param btr Number of bytes to receive (Must be an even number).
 * @return bool True if the packet was received successfully, false otherwise.
 */
static bool rcvDataBlock(uint8_t *buff, uint32_t btr) {
  // Assume CS is already asserted

  if (btr % 2 != 0)  // Must be an even number
    return false;

  uint8_t token = 0xFF;

  /* Wait for a data packet */
  for (uint8_t i = 0; i < SDC_ACTION_NUM_ATTEMPTS_DEFAULT; i++) {
    spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, &token);
    if (token != 0xFF) break;
  }

  /* If not valid data token, return with error */
  if (token != SDC_CMD17_DATA_TOKEN) return false;

  while (btr) {
    spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, buff++);
    spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, buff++);
  }

  /* Discard CRC */
  unsigned char crc;
  spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, &crc);
  spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, &crc);
}
