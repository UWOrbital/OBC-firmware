#pragma once

#include "obc_errors.h"
#include "spi.h"

obc_error_code_t spiDmaInit(spiBASE_t *spiReg);

obc_error_code_t dmaSpiTransmitandReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint16_t *txData,
                                               uint16_t *rxData, size_t dataLen);

void dmaSpi1FinishedCallback(void);
