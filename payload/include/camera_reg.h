#include "stdint.h"

void write_reg(uint16_t addr, uint16_t data);
void read_reg(uint16_t addr, uint16_t *rx_data);