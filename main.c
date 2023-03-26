#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include "system.h"
#include <stdio.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>

#include "arducam.h"
#include "camera_reg.h"

#define TEST_REG 0x00
#define VERSION_REG 0x40 // RM46: 96, Arduino: 128

#define I2C_REG i2cREG1

int main(void) {

    // Run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();
    sciInit();
    i2cInit();
    spiInit();

    // Initialize bus mutexes
    initSpiMutex();
    initSciMutex();
    initI2CMutex();

    uint16_t tx_data = 0;
    uint16_t rx_data = 7;

    gioSetDirection(gioPORTA, 0xFFFF); // set all Port A pins as output
    gioSetBit(gioPORTA, 0, 1); // set bit 2 on Port A to high

    // set_format(JPEG);
    // InitCAM();

    // write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
    // OV5642_set_JPEG_size(OV5642_320x240);
    // delay(1000);
    // clear_fifo_flag();
    // write_reg(ARDUCHIP_FRAMES, 0x00);

    while(1) {
        uint8_t reg_data[2] = {0x30, 0x0b};
        uint8_t rx_reg = 0;

        // Toggle the LED.
        gioToggleBit(gioPORTB, 1);
        
        // /* START OF I2C SEND */
        // i2cSetSlaveAdd(I2C_REG, 0x3C);
        // i2cSetDirection(I2C_REG, I2C_TRANSMITTER);
        // i2cSetCount(I2C_REG, 1);
        // i2cSetMode(I2C_REG, I2C_MASTER);
        // i2cSetStop(I2C_REG);
        // i2cSetStart(I2C_REG);
        // i2cSend(I2C_REG, 1, &tx_data);

        // while(i2cIsBusBusy(I2C_REG));

        // // while(!i2cIsStopDetected(I2C_REG));

        // i2cClearSCD(I2C_REG);
        // /* END OF I2C */

        // i2cSendTo(0x3C, 1, &tx_data);
        // for (int i = 0; i < 4000000; i++) {
        //     // Do nothing.
        // }
        // i2cSendTo(0x3C, 2, &reg_data);
        // i2cReceiveFrom(0x3C, 2, &rx_reg);

        write_reg(TEST_REG, tx_data);
        for (int i = 0; i < 4000000; i++) {
            // Do nothing.
        }
        read_reg(TEST_REG, &rx_data);

        char buffer[50];
        uint8_t len = sprintf(buffer, "\r\nrx_data: %d", rx_data);
        sciPrintText((unsigned char *)buffer, len);

        if(++tx_data >= 5) {
            tx_data = 0;
        }

        // Simple delay.
        for (int i = 0; i < 10000000; i++) {
            // Do nothing.
        }
    }

}