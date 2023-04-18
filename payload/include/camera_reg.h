#include "stdint.h"

void write_reg(uint16_t addr, uint16_t data);
void read_reg(uint16_t addr, uint16_t *rx_data);

void wrSensorReg16_8(int regID, int regDat);
void rdSensorReg16_8(int regID, int* regDat);

uint8_t get_bit(uint8_t addr, uint8_t bit);