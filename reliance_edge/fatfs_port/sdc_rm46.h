#ifndef SDC_RM46_H_
#define SDC_RM46_H_

#include <sys_common.h>

/* SD Card SPI Config */
#define SDC_SPI_PORT         spiPORT3
#define SDC_SPI_REG          spiREG3
#define SDC_SPI_CS           1

typedef enum {
    POWER_OFF,
    POWER_ON,
} sdc_power_t;

#endif /* SDC_RM46_H_ */