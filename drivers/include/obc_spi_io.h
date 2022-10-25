#ifndef DRIVERS_INCLUDE_OBC_SPI_IO_H_
#define DRIVERS_INCLUDE_OBC_SPI_IO_H_

#include <stdint.h>

#include <spi.h>
#include <gio.h>

// Note that SPI2 isn't available on the RM46 PGE package, so there's
// technically only 4 SPI ports available on the RM46.
#define NUM_SPI_PORTS 5

void spiMutexInit(void);

/**
 * @brief Deselect the SD card's SPI chip select.
 */
uint8_t deassertChipSelect(gioPORT_t *spiPort, uint8_t csNum);

/**
 * @brief Select the SD card's SPI chip select.
 */
uint8_t assertChipSelect(gioPORT_t *spiPort, uint8_t csNum);

uint8_t spiTransmitAndReceiveByte(spiBASE_t *spiReg, unsigned char outb, unsigned char *inb);

uint8_t spiTransmitByte(spiBASE_t *spiReg, unsigned char outb);

unsigned char spiReceiveByte(spiBASE_t *spiReg, unsigned char *inb);

#endif // DRIVERS_INCLUDE_OBC_SPI_IO_H_