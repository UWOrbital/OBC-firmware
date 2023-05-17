#include <spi.h>
#include <gio.h>
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_board_config.h"

#include "ov5642_regs.h"
#include "camera_reg.h"

#define CAM_I2C_WR_ADDR 0x3C
#define CAM_I2C_RD_ADDR 0x3C

#define TCA_I2C_ADDR 0x70

static cam_settings_t cam_config[] = {
    [PRIMARY] = {   .spi_config = {
                        .CS_HOLD = FALSE,
                        .WDEL = FALSE,
                        .DFSEL = SPI_FMT_0,
                        .CSNR = SPI_CS_NONE },
                    .cs_num = CAM_CS_1 },
    [SECONDARY] = {   .spi_config = {
                        .CS_HOLD = FALSE,
                        .WDEL = FALSE,
                        .DFSEL = SPI_FMT_0,
                        .CSNR = SPI_CS_NONE },
                    .cs_num = CAM_CS_2 },
};

obc_error_code_t camWriteReg(uint8_t addr, uint8_t data, camera_t cam) {
    obc_error_code_t errCode;
    uint8_t datas[2] = {0x80, 0x55};
    RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    addr = addr | 0x80;
    spiTransmitBytes(CAM_SPI_REG, &cam_config[cam].spi_config, datas, 2);
    // RETURN_IF_ERROR_CODE(spiTransmitByte(CAM_SPI_REG, &cam_config[cam].spi_config, addr));
    // RETURN_IF_ERROR_CODE(spiTransmitByte(CAM_SPI_REG, &cam_config[cam].spi_config, data));
    RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    return errCode;
}

obc_error_code_t camReadReg(uint8_t addr, uint8_t *rx_data, camera_t cam) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    addr = addr & 0x7F;
    RETURN_IF_ERROR_CODE(spiTransmitByte(CAM_SPI_REG, &cam_config[cam].spi_config, addr));
    RETURN_IF_ERROR_CODE(spiReceiveByte(CAM_SPI_REG, &cam_config[cam].spi_config, rx_data));
    RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    return errCode;
}

obc_error_code_t camWriteByte(uint8_t byte, camera_t cam) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    RETURN_IF_ERROR_CODE(spiTransmitByte(CAM_SPI_REG, &cam_config[cam].spi_config, byte));
    RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    return errCode;
}

obc_error_code_t camReadByte(uint8_t *byte, camera_t cam) {
    return spiReceiveByte(CAM_SPI_REG, &cam_config[cam].spi_config, byte);
}

obc_error_code_t camWriteSensorReg16_8(uint32_t regID, uint8_t regDat) {
    uint8_t reg_tx_data[3] = { (regID >> 8), (regID & 0x00FF), regDat };
	return i2cSendTo(CAM_I2C_WR_ADDR, 3, reg_tx_data);
}

obc_error_code_t camReadSensorReg16_8(uint8_t regID, uint8_t* regDat) {
    // Todo: regID is byteswapped for some reason so 0x3138 needs to be input as 0x3831
    obc_error_code_t errCode;
	RETURN_IF_ERROR_CODE(i2cSendTo(0x3C, 2, &regID));
	RETURN_IF_ERROR_CODE(i2cReceiveFrom(0x3C, 1, regDat));
    return errCode;
}

obc_error_code_t camWriteSensorRegs16_8(const sensor_reg_t reglist[]) {
    obc_error_code_t errCode;
    int reg_addr = 0;
    int reg_val = 0;
    const sensor_reg_t *next = reglist;

    while (!errCode && ((reg_addr != 0xffff) || (reg_val != 0xff))) {
        reg_addr = next->reg;
        reg_val = next->val;
        errCode = camWriteSensorReg16_8(reg_addr, reg_val);
        next++;
    }
    return errCode;
}

obc_error_code_t tcaSelect(uint8_t tca) {
    if (tca > 7) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    tca = (1 << tca);
    return i2cSendTo(TCA_I2C_ADDR, 1, &tca);
}

uint8_t getBit(uint8_t addr, uint8_t bit, camera_t cam)
{
  uint8_t temp;
  camReadReg(addr, &temp, cam);
  temp = temp & (1 << bit);
  return temp;
}
