#include "mpu6050.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include "stdio.h"

void mpu6050_wakeup(void) {
    uint8_t data = 0x00;
    sci_print_text((uint8_t*)"Trying to wake up MPU6050\r\n", 28);
    i2c_write_register(MPU6050_DEFAULT_ADDRESS, MPU6050_REG_PWR_MGMT_1, &data, 1);
    sci_print_text((uint8_t*)"MPU6050 Woken Up\r\n", 19);
}

uint8_t mpu6050_read_accel_data(double *accX, double *accY, double *accZ) {
    uint8_t data[6];
    if (i2c_read_register(MPU6050_DEFAULT_ADDRESS, MPU6050_REG_ACCEL_XOUT_H, data, 6) == 0) {
        sci_print_text((uint8_t*)"Failed to read acceleration data\r\n", 35);
        return 0;
    }
    
    *accX = (int16_t)((data[0] << 8) | data[1]) * (9.81 / 16384.0);
    *accY = (int16_t)((data[2] << 8) | data[3]) * (9.81 / 16384.0);
    *accZ = (int16_t)((data[4] << 8) | data[5]) * (9.81 / 16384.0);

    char buf[31] = {};
    sprintf(buf, "X: %0.02f Y: %0.02f Z: %0.02f\r\n", *accX, *accY, *accZ);
    sci_print_text((uint8_t*)buf, sizeof(buf));

    return 1;
}