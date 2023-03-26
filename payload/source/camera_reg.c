#include <spi.h>
#include <gio.h>
#include "obc_i2c_io.h"
#include "obc_spi_io.h"

#include "ov5642_regs.h"
#include "camera_reg.h"

#define SPI_REG spiREG3
#define CS_NUM 1

#define CAM_I2C_WR_ADDR 0x78
#define CAM_I2C_RD_ADDR 0x79

spiDAT1_t spi_config = {
    .CS_HOLD = FALSE,
    .WDEL = FALSE,
    .DFSEL = SPI_FMT_0,
    .CSNR = SPI_CS_NONE
};

void write_reg(uint16_t addr, uint16_t data) {
    // gioSetBit(gioPORTA, 0, 0);
    // addr = addr | 0x80;
    // spiTransmitByte(SPI_REG, &spi_config, &addr);
    // spiTransmitByte(SPI_REG, &spi_config, data);
    // // Simple delay.
    // for (int i = 0; i < 25; i++) {
    //     // Do nothing.
    // }
    // gioSetBit(gioPORTA, 0, 1);

    gioSetBit(gioPORTA, 0, 0);
    addr = addr | 0x80;
    spiTransmitData(SPI_REG, &spi_config, 1, &addr);
    spiTransmitData(SPI_REG, &spi_config, 1, &data);
    // Simple delay.
    for (int i = 0; i < 25; i++) {
        // Do nothing.
    }
    gioSetBit(gioPORTA, 0, 1);
}

void read_reg(uint16_t addr, uint16_t *rx_data) {
    // gioSetBit(gioPORTA, 0, 0);
    // addr = addr & 0x7F;
    // spiTransmitByte(SPI_REG, &spi_config, &addr);
    // spiReceiveByte(SPI_REG, &spi_config, &rx_data);
    // // Simple delay.
    // for (int i = 0; i < 25; i++) {
    //     // Do nothing.
    // }
    // gioSetBit(gioPORTA, 0, 1);

    gioSetBit(gioPORTA, 0, 0);
    addr = addr & 0x7F;
    spiTransmitData(SPI_REG, &spi_config, 1, &addr);
    spiReceiveData(SPI_REG, &spi_config, 1, rx_data);
    // Simple delay.
    for (int i = 0; i < 25; i++) {
        // Do nothing.
    }
    gioSetBit(gioPORTA, 0, 1);
}

void wrSensorReg16_8(int regID, int regDat) {
  i2cSendTo(CAM_I2C_WR_ADDR, 1, regID >> 8); // sends instruction byte, MSB first
  i2cSendTo(CAM_I2C_WR_ADDR, 1, regID & 0x00FF);
  i2cSendTo(CAM_I2C_WR_ADDR, 1, regDat & 0x00FF);
}

void wrSensorRegs16_8(const struct sensor_reg reglist[]) {
    int err = 0;
    unsigned int reg_addr;
    unsigned char reg_val;
    const struct sensor_reg *next = reglist;

    while ((reg_addr != 0xffff) | (reg_val != 0xff)) {
        reg_addr = &next->reg;
        reg_val = &next->val;
        wrSensorReg16_8(reg_addr, reg_val);
        next++;
    }
	return 1;
}