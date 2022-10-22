#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "sys_common.h"
#include "gio.h"
#include "spi.h"

#include "diskio.h"
#include "sdc_rm46.h"

/*---------------------------------------------*/
/* SPI-related R/W functions                   */
/*---------------------------------------------*/

/**
 * @brief Deselect the SD card's SPI chip select.
 */
static void DESELECT (void) {
    SDC_SPI_PORT->DSET = 1 << SDC_SPI_CS;
}

/**
 * @brief Select the SD card's SPI chip select.
 */
static void SELECT (void) {
    SDC_SPI_PORT->DCLR = 1 << SDC_SPI_CS;
}

/**
 * @brief Read and write a byte to the SD card's SPI interface.
*/
static unsigned char spiSendAndReceiveByte(unsigned char outb) {
    while ((SDC_SPI_REG->FLG & 0x0200) == 0); // Wait until TXINTFLG is set for previous transmission
    SDC_SPI_REG->DAT1 = outb | 0x100D0000;    // transmit register address

    while ((SDC_SPI_REG->FLG & 0x0100) == 0); // Wait until RXINTFLG is set when new value is received
    return((unsigned char)SDC_SPI_REG->BUF);  // Return received value
}

/**
 * @brief Transmit a byte to the SD card's SPI interface.
 * @param dat Byte to transmit.
 */
static void spiSendByte(BYTE dat) {
    unsigned int ui32RcvDat;

    while ((SDC_SPI_REG->FLG & 0x0200) == 0); // Wait until TXINTFLG is set for previous transmission
    SDC_SPI_REG->DAT1 = dat | 0x100D0000;    // transmit register address

    while ((SDC_SPI_REG->FLG & 0x0100) == 0); // Wait until RXINTFLG is set when new value is received
    ui32RcvDat = SDC_SPI_REG->BUF;  // to get received value
    ui32RcvDat = ui32RcvDat;        // to avoid compiler warning
}

/**
 * @brief Receive a byte from the SD card's SPI interface.
 * 
 * @return BYTE Received byte.
 */
static BYTE spiReceiveByte(void)
{
    while ((SDC_SPI_REG->FLG & 0x0200) == 0); // Wait until TXINTFLG is set for previous transmission
    SDC_SPI_REG->DAT1 = 0xFF | 0x100D0000;    // transmit register address

    while ((SDC_SPI_REG->FLG & 0x0100) == 0); // Wait until RXINTFLG is set when new value is received
    return((unsigned char)SDC_SPI_REG->BUF);  // Return received value
}


/*---------------------------------------------*/
/* SD Card Private Functions                   */
/*---------------------------------------------*/

static volatile DSTATUS stat = STA_NOINIT;    /* Disk status */
static volatile BYTE timer1, timer2;    /* 100Hz decrement timer */
static BYTE cardType;            /* b0:MMC, b1:SDC, b2:Block addressing */
static sdc_power_t powerFlag = POWER_OFF;    /* indicates if "power" is on */

/**
 * @brief Check if card is ready
 * 
 * @return bool True if card is ready, false otherwise.
 */
static bool isCardReady(void) {
    BYTE res;

    timer2 = 50;    /* Wait for ready in timeout of 500ms */
    spiReceiveByte();
    do
        res = spiReceiveByte();
    while ((res != 0xFF) && timer2);

    return (res != 0xFF);
}


/**
 * @brief Send >74 clock transitions with CS and DI held high. This is
 * required after card power up to get it into SPI mode.
 */
static void sendClockTrain(void) {   
    DESELECT();   /* CS = H */

    for(int i = 0 ; i < 10 ; i++) {
        spiSendAndReceiveByte(0xFF);
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

    timer1 = 100;
    do {
        token = spiReceiveByte();
    } while ((token == 0xFF) && timer1);
    if(token != 0xFE) return FALSE; /* If not valid data token, retutn with error */

    do {    /* Receive the data block into buffer */
        *buff = spiReceiveByte(); buff++;
        *buff = spiReceiveByte(); buff++;
    } while (btr -= 2);
    spiReceiveByte();   /* Discard CRC */
    spiReceiveByte();

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

    spiSendByte(token); // Send token
    if (token != 0xFD) { // Is data token
        for (int wc = 0; wc < 512; wc++) { // Send the data block to the MMC
            spiSendByte(*buff++);
        }
        spiSendByte(0xFF); /* CRC (Dummy) */
        spiSendByte(0xFF);
        
        BYTE resp = spiReceiveByte(); /* Reveive data response */
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
    spiSendByte(cmd);                      /* Command */
    spiSendByte((BYTE)(arg >> 24));        /* Argument[31..24] */
    spiSendByte((BYTE)(arg >> 16));        /* Argument[23..16] */
    spiSendByte((BYTE)(arg >> 8));         /* Argument[15..8] */
    spiSendByte((BYTE)arg);                /* Argument[7..0] */

    BYTE crc = 0xFF;
    if (cmd == CMD0) crc = 0x95;            /* CRC for CMD0(0) */
    if (cmd == CMD8) crc = 0x87;            /* CRC for CMD8(0x1AA) */
    spiSendByte(crc);


    /* Receive command response */
    if (cmd == CMD12) spiReceiveByte();        /* Skip a stuff byte when stop reading */
    
    BYTE res = spiReceiveByte();
    for (int attempt = 0; attempt < 9 && (res & 0x80); attempt++) {
        res = spiReceiveByte();
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
    spiSendByte(CMD12);
    spiSendByte(0);
    spiSendByte(0);
    spiSendByte(0);
    spiSendByte(0);
    spiSendByte(0);

    /* Data transfer stops 2 bytes after 6-byte CMD12 */
    spiReceiveByte(); spiReceiveByte();

    /* SDC should now send 2-6 0xFF bytes, the response byte, and then another 0xFF */
    /* Some cards don't send the 2-6 0xFF bytes */
    for(n = 0; n < 8; n++) {
        val = spiReceiveByte();
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

    SELECT();                /* CS = L */
    ty = 0;

    while (sendCMD(CMD0, 0) != 1); // TODO: Add timer to prevent infinite loop
    
    timer1 = 100;                        /* Initialization timeout of 1000 msec */
    if (sendCMD(CMD8, 0x1AA) == 1) {    /* SDC Ver2+ */
        for (n = 0; n < 4; n++) ocr[n] = spiReceiveByte();
        if (ocr[2] == 0x01 && ocr[3] == 0xAA) {    /* The card can work at vdd range of 2.7-3.6V */
            do {
                if (sendCMD(CMD55, 0) <= 1 && sendCMD(CMD41, 1UL << 30) == 0)    break;    /* ACMD41 with HCS bit */
            } while (timer1);
            if (timer1 && sendCMD(CMD58, 0) == 0) {    /* Check CCS bit */
                for (n = 0; n < 4; n++) ocr[n] = spiReceiveByte();
                ty = (ocr[0] & 0x40) ? 6 : 2;
            }
        }
    } else {                            /* SDC Ver1 or MMC */
        ty = (sendCMD(CMD55, 0) <= 1 && sendCMD(CMD41, 0) <= 1) ? 2 : 1;    /* SDC : MMC */
        do {
            if (ty == 2) {
                if (sendCMD(CMD55, 0) <= 1 && sendCMD(CMD41, 0) == 0) break;    /* ACMD41 */
            } else {
                if (sendCMD(CMD1, 0) == 0) break;                                /* CMD1 */
            }
        } while (timer1);
        if (!timer1 || sendCMD(CMD16, 512) != 0)    /* Select R/W block length */
            ty = 0;
    }

    cardType = ty;
    DESELECT();            /* CS = H */
    spiReceiveByte();            /* Idle (Release DO) */

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

    SELECT();
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

    DESELECT();            /* CS = H */
    spiReceiveByte();            /* Idle (Release DO) */

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

    SELECT();

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
    DESELECT();
    spiReceiveByte();

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
        
        SELECT();

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
        DESELECT();
        spiReceiveByte();
    }

    return res;
}

/**
 * @brief Timer interrupt procedure.
 * @note This function must be called in period of 10ms.
 */
void disk_timerproc(void) {
    BYTE n;

    n = timer1;
    if (n) timer1 = --n;
    n = timer2;
    if (n) timer2 = --n;
}

/**
 * @brief Get the current time.
 * @return DWORD Current time
 */
DWORD get_fattime (void) {
    return    ((2007UL-1980) << 25) // Year = 2007
            | (6UL << 21)           // Month = June
            | (5UL << 16)           // Day = 5
            | (11U << 11)           // Hour = 11
            | (38U << 5)            // Min = 38
            | (0U >> 1);            // Sec = 0
}