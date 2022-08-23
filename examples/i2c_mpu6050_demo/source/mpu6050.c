#include "mpu6050.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include <sci.h>
#include <stdio.h>

void wakeupMPU6050(void) {
    uint8_t data = 0x00;
    printTextSci(scilinREG, (unsigned char *)"Trying to wake up MPU6050\r\n", 28);
    i2cWriteReg(MPU6050_DEFAULT_ADDRESS, MPU6050_REG_PWR_MGMT_1, &data, 1);
    printTextSci(scilinREG, (unsigned char *)"MPU6050 Woken Up\r\n", 19);
}

uint8_t readAccelDataMPU6050(double *accX, double *accY, double *accZ) {
    uint8_t data[6];
    if (i2cReadReg(MPU6050_DEFAULT_ADDRESS, MPU6050_REG_ACCEL_XOUT_H, data, 6) == 0) {
        printTextSci(scilinREG, (unsigned char *)"Failed to read acceleration data\r\n", 35);
        return 0;
    }

    *accX = (int16_t)((data[0] << 8) | data[1]) * (9.81 / 16384.0);
    *accY = (int16_t)((data[2] << 8) | data[3]) * (9.81 / 16384.0);
    *accZ = (int16_t)((data[4] << 8) | data[5]) * (9.81 / 16384.0);

    char buf[31] = {};
    sprintf(buf, "X: %0.02f Y: %0.02f Z: %0.02f\r\n", *accX, *accY, *accZ);
    printTextSci(scilinREG, (unsigned char *)buf, sizeof(buf));

    return 1;
} 