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
                        .CS_HOLD = false,
                        .WDEL = false,
                        .DFSEL = CAM_SPI_DATA_FORMAT,
                        .CSNR = SPI_CS_NONE },
                    .cs_num = CAM_CS_1 },
    [SECONDARY] = {   .spi_config = {
                        .CS_HOLD = false,
                        .WDEL = false,
                        .DFSEL = CAM_SPI_DATA_FORMAT,
                        .CSNR = SPI_CS_NONE },
                    .cs_num = CAM_CS_2 },
};

obc_error_code_t camWriteReg(uint8_t addr, uint8_t data, camera_t cam) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    addr = addr | 0x80;
    uint8_t tx[2] = {addr, data};
    spiTransmitBytes(CAM_SPI_REG, &cam_config[cam].spi_config, tx, 2);
    RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    return errCode;
}

obc_error_code_t camReadReg(uint8_t addr, uint8_t *rx_data, camera_t cam) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    addr = addr & 0x7F;
    errCode = spiTransmitByte(CAM_SPI_REG, &cam_config[cam].spi_config, addr);
    if(!errCode) {
        errCode = spiReceiveByte(CAM_SPI_REG, &cam_config[cam].spi_config, rx_data);
    }
    RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    return errCode;
}

obc_error_code_t camWriteByte(uint8_t byte, camera_t cam) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
    errCode = spiTransmitByte(CAM_SPI_REG, &cam_config[cam].spi_config, byte);
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

obc_error_code_t camWriteSensorRegs16_8(const sensor_reg_t reglist[], uint16_t reglistLen) {
    obc_error_code_t errCode;

    for (int i = 0; i < reglistLen; i++) {
        RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(reglist[i].reg, reglist[i].val));
    }
    return errCode;
}

obc_error_code_t tcaSelect(camera_t cam) {
    uint8_t tca = 0;
    if (cam == PRIMARY) {
        tca = (1 << 0);
    } else {
        tca = (1 << 1);
    }
    return i2cSendTo(TCA_I2C_ADDR, 1, &tca);
}

uint8_t getBit(uint8_t addr, uint8_t bit, camera_t cam)
{
  uint8_t temp;
  camReadReg(addr, &temp, cam);
  temp = temp & (1 << bit);
  return temp;
}
