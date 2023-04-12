#include "stdint.h"

#define SPI_REG spiREG3
#define CS_NUM 1


spiDAT1_t spi_config = {
    .CS_HOLD = FALSE,
    .WDEL = FALSE,
    .DFSEL = SPI_FMT_0,
    .CSNR = SPI_CS_NONE
};

void write_reg(uint16_t addr, uint16_t data);
void read_reg(uint16_t addr, uint16_t *rx_data);