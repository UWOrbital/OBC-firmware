#ifndef SDC_RM46_H_
#define SDC_RM46_H_

#include <sys_common.h>

/* SD Card SPI Config */
#define SDC_SPI_PORT         spiPORT3
#define SDC_SPI_REG          spiREG3
#define SDC_SPI_CS           1

#define CHECK_READY_WAIT pdMS_TO_TICKS(500)
#define RCV_DATA_WAIT pdMS_TO_TICKS(500)

/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND */
#define CMD8    (0x40+8)    /* SEND_IF_COND */
#define CMD9    (0x40+9)    /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */

typedef enum {
    POWER_OFF,
    POWER_ON,
    POWER_GET
} sdc_power_t;

#endif /* SDC_RM46_H_ */