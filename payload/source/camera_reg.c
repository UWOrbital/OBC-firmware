#include <spi.h>
#include <gio.h>
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include "ov5642_regs.h"
#include "camera_reg.h"

spiDAT1_t spi_config = {
    .CS_HOLD = FALSE,
    .WDEL = FALSE,
    .DFSEL = SPI_FMT_0,
    .CSNR = SPI_CS_NONE
};

obc_error_code_t write_reg(uint16_t addr, uint16_t data) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(SPI_PORT, 1));
    addr = addr | 0x80;
    RETURN_IF_ERROR_CODE(spiTransmitByte(SPI_REG, &spi_config, &addr));
    RETURN_IF_ERROR_CODE(spiTransmitByte(SPI_REG, &spi_config, data));
    RETURN_IF_ERROR_CODE(deassertChipSelect(SPI_PORT, 1));
}

obc_error_code_t read_reg(uint16_t addr, uint16_t *rx_data) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(SPI_PORT, 1));
    addr = addr & 0x7F;
    RETURN_IF_ERROR_CODE(spiTransmitByte(SPI_REG, &spi_config, &addr));
    RETURN_IF_ERROR_CODE(spiReceiveByte(SPI_REG, &spi_config, &rx_data));
    RETURN_IF_ERROR_CODE(deassertChipSelect(SPI_PORT, 1));
}

obc_error_code_t wrSensorReg16_8(int regID, int regDat) {
    uint8_t reg_tx_data[3] = { (regID >> 8), (regID & 0x00FF), (regDat & 0x00FF)};
	i2cSendTo(CAM_I2C_WR_ADDR, 3, reg_tx_data);
}

obc_error_code_t rdSensorReg16_8(int regID, int* regDat) {
    // Todo: regID is byteswapped for some reason so 0x3138 needs to be input as 0x3831
    obc_error_code_t errCode;
	RETURN_IF_ERROR_CODE(i2cSendTo(0x3C, 2, &regID));
	RETURN_IF_ERROR_CODE(i2cReceiveFrom(0x3C, 1, &regDat));
}

obc_error_code_t wrSensorRegs16_8(const struct sensor_reg reglist[]) {
    obc_error_code_t errCode;
    int reg_addr = 0;
    int reg_val = 0;
    const struct sensor_reg *next = reglist;

    while (!errCode && ((reg_addr != 0xffff) || (reg_val != 0xff))) {
        reg_addr = next->reg;
        reg_val = next->val;
        errCode = wrSensorReg16_8(reg_addr, reg_val);
        next++;
    }
}

obc_error_code_t tca_select(uint8_t tca) {
    if (tca > 7) {
        return;
    }
    tca = (1 << tca);
    i2cSendTo(TCA_I2C_ADDR, 1, &tca);
}

uint8_t get_bit(uint8_t addr, uint8_t bit)
{
  uint8_t temp;
  read_reg(addr, &temp);
  temp = temp & (1 << bit);
  return temp;
}
