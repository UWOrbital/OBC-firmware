#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <FreeRTOS.h>
#include <os_task.h>

#include "sys_common.h"
#include "gio.h"
#include "spi.h"

#include "diskio.h"
#include "sdc_rm46.h"
#include "obc_spi_io.h"

/*---------------------------------------------*/
/* SD Card Private Functions                   */
/*---------------------------------------------*/

static volatile DSTATUS stat = STA_NOINIT;    /* Disk status */
static BYTE cardType;            /* b0:MMC, b1:SDC, b2:Block addressing */
static sdc_power_t powerFlag = POWER_OFF;    /* indicates if "power" is on */

/**
 * @brief Check if card is ready
 * 
 * @return bool True if card is ready, false otherwise.
 */
static bool isCardReady(void) {
    BYTE res;

    TickType_t endTimeTicks = xTaskGetTickCount() + pdMS_TO_TICKS(500);
    spiReceiveByte(SDC_SPI_REG, &res);
    do {
        spiReceiveByte(SDC_SPI_REG, &res);
    } while ((res != 0xFF) && (xTaskGetTickCount() < endTimeTicks));

    return (res != 0xFF);
}


/**
 * @brief Send >74 clock transitions with CS and DI held high. This is
 * required after card power up to get it into SPI mode.
 */
static void sendClockTrain(void) {   
    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);   /* CS = H */

    for(int i = 0 ; i < 10 ; i++) {
        spiTransmitByte(SDC_SPI_REG, 0xFF);
    }
}

/**
 * @brief Turn the SD card power on.
 * @warning This function doesn't actually turn the power on, it just
 * sets a flag to indicate that the power is on.
 */
static void turnOnSDC(void) {
    sendClockTrain();
    powerFlag = POWER_ON;
}

/**
 * @brief Set the max speed for the SD card's SPI interface.
 */
static void maximizeSpeed(void) {
    // todo jc 20151004 - check if this is portable between hercules controllers/clock speeds
      SDC_SPI_REG->FMT0 &= 0xFFFF00FF;  // mask out baudrate prescaler
                                        // Max. 5 MBit used for Data Transfer.
      SDC_SPI_REG->FMT0 |= 5 << 8;      // baudrate prescale 10MHz / (1+1) = 5MBit
}

/**
 * @brief Turn the SD card power off.
 * @warning This function doesn't actually turn the power off, it just
 * sets a flag to indicate that the power is off.
 */
static void turnOffSDC(void) {
    powerFlag = POWER_OFF;
}

/**
 * @brief Check the power status of the SD card.
 * 
 * @return sdc_power_t POWER_ON if the SD card is powered on, POWER_OFF otherwise.
 */
static sdc_power_t checkPower(void) {
    return powerFlag;
}

/**
 * @brief Receive a data packet from the SD card.
 * @param buff Buffer to store the received data.
 * @param btr Number of bytes to receive (Must be an even number).
 * @return bool True if the packet was received successfully, false otherwise.
 */
static bool rcvDataBlock(BYTE *buff, UINT btr) {
    BYTE token;

    if (btr % 2 != 0)
        return FALSE;

    TickType_t endTimeTicks = xTaskGetTickCount() + pdMS_TO_TICKS(1000);

    do {
        spiReceiveByte(SDC_SPI_REG, &token);
    } while ((token == 0xFF) && (xTaskGetTickCount() < endTimeTicks));

    if(token != 0xFE) return FALSE; /* If not valid data token, retutn with error */

    do {    /* Receive the data block into buffer */
        spiReceiveByte(SDC_SPI_REG, buff++);
        spiReceiveByte(SDC_SPI_REG, buff++);
    } while (btr -= 2);
    
    unsigned char crc;
    spiReceiveByte(SDC_SPI_REG, &crc);   /* Discard CRC */
    spiReceiveByte(SDC_SPI_REG, &crc);

    return TRUE;
}

#if _READONLY == 0
/**
 * @brief Send a data packet to the SD card.
 * @param buff 512 byte buffer containing the data to send.
 * @param token Data/Stop token.
 * @return bool True if the packet was sent successfully, false otherwise.
 */
static bool sendDataBlock(const BYTE *buff, BYTE token) {
    if (isCardReady()) return FALSE;

    spiTransmitByte(SDC_SPI_REG, token); // Send token
    if (token != 0xFD) { // Is data token
        for (int wc = 0; wc < 512; wc++) { // Send the data block to the MMC
            spiTransmitByte(SDC_SPI_REG, *buff++);
        }
        spiTransmitByte(SDC_SPI_REG, 0xFF); /* CRC (Dummy) */
        spiTransmitByte(SDC_SPI_REG, 0xFF);
        
        BYTE resp;
        spiReceiveByte(SDC_SPI_REG, &resp); /* Reveive data response */
        if ((resp & 0x1F) != 0x05) /* If not accepted, return with error */
            return FALSE;
    }
    return TRUE;
}
#endif /* _READONLY */

/**
 * @brief Send a command packet to the SD card.
 * @param cmd Command byte.
 * @param arg Argument.
 * @return BYTE Response byte.
 */
static BYTE sendCMD(BYTE cmd, DWORD arg) {
    if (isCardReady()) return 0xFF;

    /* Send command packet */
    spiTransmitByte(SDC_SPI_REG, cmd);                      /* Command */
    spiTransmitByte(SDC_SPI_REG, (BYTE)(arg >> 24));        /* Argument[31..24] */
    spiTransmitByte(SDC_SPI_REG, (BYTE)(arg >> 16));        /* Argument[23..16] */
    spiTransmitByte(SDC_SPI_REG, (BYTE)(arg >> 8));         /* Argument[15..8] */
    spiTransmitByte(SDC_SPI_REG, (BYTE)arg);                /* Argument[7..0] */

    BYTE crc = 0xFF;
    if (cmd == CMD0) crc = 0x95;            /* CRC for CMD0(0) */
    if (cmd == CMD8) crc = 0x87;            /* CRC for CMD8(0x1AA) */
    spiTransmitByte(SDC_SPI_REG, crc);


    /* Receive command response */
    unsigned char tmp;
    if (cmd == CMD12) spiReceiveByte(SDC_SPI_REG, &tmp);        /* Skip a stuff byte when stop reading */
    
    BYTE res;
    spiReceiveByte(SDC_SPI_REG, &res);
    for (int attempt = 0; attempt < 9 && (res & 0x80); attempt++) {
        spiReceiveByte(SDC_SPI_REG, &res);
    }

    return res;
}

/**
 * @brief Send CMD12 to the SD card to stop a multi-block read.
 * 
 * @return BYTE Response byte.
 */
static BYTE stopTransmission(void) {
    BYTE n, res, val;

    /* Send command packet - the argument for CMD12 is ignored. */
    spiTransmitByte(SDC_SPI_REG, CMD12);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);

    /* Data transfer stops 2 bytes after 6-byte CMD12 */
    spiReceiveByte(SDC_SPI_REG, &val); spiReceiveByte(SDC_SPI_REG, &val);

    /* SDC should now send 2-6 0xFF bytes, the response byte, and then another 0xFF */
    /* Some cards don't send the 2-6 0xFF bytes */
    for(n = 0; n < 8; n++) {
        spiReceiveByte(SDC_SPI_REG, &val);
        if(val != 0xFF)
            res = val;
    }

    return res;
}


/*---------------------------------------------------------------------------*/
/* Public Functions                                                          */
/*---------------------------------------------------------------------------*/

/**
 * @brief Initialize the SD card.
 * @param pdrv Physical drive number (0).
 * @return DSTATUS Status
 */
DSTATUS disk_initialize(BYTE drv){
    BYTE n, ty, ocr[4];

    if (drv) return STA_NOINIT;            /* Supports only single drive */
    if (stat & STA_NODISK) return stat;    /* No card in the socket */

    turnOnSDC();                            /* Force socket power on */

    assertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);                /* CS = L */
    ty = 0;

    TickType_t endTimeTicks = xTaskGetTickCount() + pdMS_TO_TICKS(5000);
    while ((sendCMD(CMD0, 0) != 1) && (xTaskGetTickCount() < endTimeTicks));
    
    // Even if CMD0 fails, we'll try to continue.
    if (sendCMD(CMD8, 0x1AA) == 1) {    /* SDC Ver2+ */
        for (n = 0; n < 4; n++) spiReceiveByte(SDC_SPI_REG, &ocr[n]);
        
        if (ocr[2] == 0x01 && ocr[3] == 0xAA) {    /* The card can work at vdd range of 2.7-3.6V */
            TickType_t endTimeTicks = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
            TickType_t currTimeTicks;
            do {
                if (sendCMD(CMD55, 0) <= 1 && sendCMD(CMD41, 1UL << 30) == 0)    break;    /* ACMD41 with HCS bit */
            } while ((currTimeTicks = xTaskGetTickCount()) < endTimeTicks);

            if ((currTimeTicks < endTimeTicks) && !sendCMD(CMD58, 0)) {    /* Check CCS bit */
                for (n = 0; n < 4; n++) spiReceiveByte(SDC_SPI_REG, &ocr[n]);
                ty = (ocr[0] & 0x40) ? 6 : 2;
            }
        }
    } else {                            /* SDC Ver1 or MMC */
        ty = (sendCMD(CMD55, 0) <= 1 && sendCMD(CMD41, 0) <= 1) ? 2 : 1;    /* SDC : MMC */
        TickType_t endTimeTicks = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
        TickType_t currTimeTicks;
        do {
            if (ty == 2) {
                if (sendCMD(CMD55, 0) <= 1 && sendCMD(CMD41, 0) == 0) break;    /* ACMD41 */
            } else {
                if (sendCMD(CMD1, 0) == 0) break;                                /* CMD1 */
            }
        } while ((currTimeTicks = xTaskGetTickCount()) < endTimeTicks);
        if ((currTimeTicks >= endTimeTicks) || sendCMD(CMD16, 512) != 0)    /* Select R/W block length */
            ty = 0;
    }

    cardType = ty;
    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

    unsigned char tmp;
    spiReceiveByte(SDC_SPI_REG, &tmp);            /* Idle (Release DO) */

    if (ty) {            /* Initialization succeded */
        stat &= ~STA_NOINIT;        /* Clear STA_NOINIT */
        maximizeSpeed();
    } else {            /* Initialization failed */
        turnOffSDC();
    }

    return stat;
}

/**
 * @brief Get disk status.
 * @param pdrv Physical drive number (0).
 * @return DSTATUS Status
 */
DSTATUS disk_status(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;        /* Supports only single drive */
    return stat;
}

/**
 * @brief Read sector(s) from the SD card.
 * @param pdrv Physical drive number (0).
 * @param buff Pointer to the data buffer to store read data.
 * @param sector Start sector number (LBA).
 * @param count Sector count (1..255).
 * @return DRESULT Result
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
    if (pdrv || !count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    if (!(cardType & 4)) sector *= 512;    /* Convert to byte address if needed */

    assertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);
    if (count == 1) {    /* Single block read */
        if ((sendCMD(CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
            && rcvDataBlock(buff, 512))
            count = 0;
    }
    else {                /* Multiple block read */
        if (sendCMD(CMD18, sector) == 0) {    /* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvDataBlock(buff, 512)) break;
                buff += 512;
            } while (--count);
            stopTransmission();                /* STOP_TRANSMISSION */
        }
    }

    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);            /* CS = H */

    unsigned char tmp;
    spiReceiveByte(SDC_SPI_REG, &tmp);            /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}

#if _READONLY == 0
/**
 * @brief Write sector(s) to the SD card.
 * @param pdrv Physical drive number (0).
 * @param buff Pointer to the data to be written.
 * @param sector Start sector number (LBA).
 * @param count Sector count (1..255).
 * @return DRESULT Result
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
    if (pdrv || !count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;
    if (stat & STA_PROTECT) return RES_WRPRT;

    if (!(cardType & 4)) sector *= 512;    /* Convert to byte address if needed */

    assertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

    if (count == 1) {    /* Single block write */
        if ((sendCMD(CMD24, sector) == 0)    /* WRITE_BLOCK */
            && sendDataBlock(buff, 0xFE))
            count = 0;
    }
    else {                /* Multiple block write */
        if (cardType & 2) {
            sendCMD(CMD55, 0); sendCMD(CMD23, count);    /* ACMD23 */
        }
        if (sendCMD(CMD25, sector) == 0) {    /* WRITE_MULTIPLE_BLOCK */
            do {
                if (!sendDataBlock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (!sendDataBlock(0, 0xFD))    /* STOP_TRAN token */
                count = 1;
        }
    }
    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

    unsigned char tmp;
    spiReceiveByte(SDC_SPI_REG, &tmp);

    return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY */

/**
 * @brief Disk I/O control.
 * @param pdrv Physical drive number (0).
 * @param cmd Control command code.
 * @param buff Pointer to the control data.
 * @return DRESULT Result
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE ctrl, void *buff) {
    DRESULT res;
    BYTE n, csd[16], *ptr = buff;
    WORD csize;

    if (pdrv) return RES_PARERR;

    res = RES_ERROR;

    if (ctrl == CTRL_POWER) {
        switch (*ptr) {
            case 0:        /* Sub control code == 0 (POWER_OFF) */
                if (checkPower() == POWER_ON)
                    turnOffSDC();
                res = RES_OK;
                break;
            case 1:        /* Sub control code == 1 (POWER_ON) */
                turnOnSDC();                /* Power on */
                res = RES_OK;
                break;
            case 2:        /* Sub control code == 2 (POWER_GET) */
                *(ptr+1) = (BYTE)checkPower();
                res = RES_OK;
                break;
            default :
                res = RES_PARERR;
        }
    } else {
        if (stat & STA_NOINIT) return RES_NOTRDY;
        
        assertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

        switch (ctrl) {
            case GET_SECTOR_COUNT :    /* Get number of sectors on the disk (DWORD) */
                if ((sendCMD(CMD9, 0) == 0) && rcvDataBlock(csd, 16)) {
                    if ((csd[0] >> 6) == 1) {    /* SDC ver 2.00 */
                        csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                        *(DWORD*)buff = (DWORD)csize << 10;
                    } else {                    /* MMC or SDC ver 1.XX */
                        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                        csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                        *(DWORD*)buff = (DWORD)csize << (n - 9);
                    }
                    res = RES_OK;
                }
                break;
            case GET_SECTOR_SIZE :    /* Get sectors on the disk (WORD) */
                *(WORD*)buff = 512;
                res = RES_OK;
                break;
            case CTRL_SYNC :    /* Make sure that data has been written */
                if (!isCardReady())
                    res = RES_OK;
                break;
            default:
                res = RES_PARERR;
        }
        deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

        unsigned char tmp;
        spiReceiveByte(SDC_SPI_REG, &tmp);
    }

    return res;
}

/**
 * @brief Get the current time.
 * @return DWORD Current time
 */
DWORD get_fattime (void) {
    return    ((2022UL-1980) << 25) // Year = 2022
            | (10UL << 21)          // Month = October
            | (23UL << 16)          // Day = 23
            | (4U << 11)            // Hour = 4
            | (31U << 5)            // Min = 31
            | (0U >> 1);            // Sec = 0
}