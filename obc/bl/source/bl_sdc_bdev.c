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
#define SDC_MOSI_HIGH 0xFFU  // Keep MOSI high during read operations

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


/*---------------------------------------------*/
/* SD Card Private Functions                   */
/*---------------------------------------------*/

/**
 * @brief Check if card is ready
 *
 * @return bool True if card is ready, false otherwise.
 */
static bool blIsCardReady(void) {

    for (uint8_t i = 0; i < SDC_ACTION_NUM_ATTEMPTS_DEFAULT; i++) {
        uint16_t res = 0;
        uint32_t spiErr = spiTransmitAndReceiveData(SDC_SPI_REG, &sdcSpiConfig, 1, &SDC_MOSI_HIGH, &res);

        if (res == 0xFF) return true;
    }

    return false;
}