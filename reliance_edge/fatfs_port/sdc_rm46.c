#include <stdint.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <spi.h>

#include "diskio.h"
#include "sdc_rm46.h"
#include "obc_spi_io.h"

/*---------------------------------------------*/
/* SD Card Definitions                         */
/*---------------------------------------------*/

#define SDC_CMD_BASE       0x40U
#define SDC_CMD0    (SDC_CMD_BASE)        /* GO_IDLE_STATE */
#define SDC_CMD1    (SDC_CMD_BASE)      /* SEND_OP_COND */
#define SDC_CMD8    (SDC_CMD_BASE+8)    /* SEND_IF_COND */
#define SDC_CMD9    (SDC_CMD_BASE+9)    /* SEND_CSD */
#define SDC_CMD10   (SDC_CMD_BASE+10)   /* SEND_CID */
#define SDC_CMD12   (SDC_CMD_BASE+12)   /* STOP_TRANSMISSION */
#define SDC_CMD16   (SDC_CMD_BASE+16)   /* SET_BLOCKLEN */
#define SDC_CMD17   (SDC_CMD_BASE+17)   /* READ_SINGLE_BLOCK */
#define SDC_CMD18   (SDC_CMD_BASE+18)   /* READ_MULTIPLE_BLOCK */
#define SDC_CMD23   (SDC_CMD_BASE+23)   /* SET_BLOCK_COUNT */
#define SDC_CMD24   (SDC_CMD_BASE+24)   /* WRITE_BLOCK */
#define SDC_CMD25   (SDC_CMD_BASE+25)   /* WRITE_MULTIPLE_BLOCK */
#define SDC_CMD41   (SDC_CMD_BASE+41)   /* SEND_OP_COND (ACMD) */
#define SDC_CMD55   (SDC_CMD_BASE+55)   /* APP_CMD */
#define SDC_CMD58   (SDC_CMD_BASE+58)   /* READ_OCR */

#define SD_SECTOR_SIZE 512U
#define SD_STOP_TRANSMISSION 0xFDU
#define SD_DATA_RESPONSE_MASK 0x1FU
#define SD_DATA_RESPONSE_ACCEPTED 0x05U

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

    const uint8_t maxTries = 100U;
    for (uint8_t i = 0; i < maxTries; i++) {
        spiReceiveByte(SDC_SPI_REG, &res);
        if (res == 0xFF)
            return true;
    }

    return false;
}


/**
 * @brief Send >74 clock transitions with CS and DI held high. This is
 * required after card power up to get it into SPI mode.
 */
static void sendClockTrain(void) {   
    const uint8_t numClkTransitions = 80U, numBytes = numClkTransitions / 8U;
    
    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

    for (uint8_t i = 0; i < numBytes; i++) {
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
    const uint8_t dataToken = 0xFEU; // Data token for single block read
    if (btr % 2 != 0) // Must be an even number
        return false;

    BYTE token = 0xFF;

    /* Wait for a data packet */
    const uint8_t maxTries = 100;
    for (uint8_t i = 0; i < maxTries; i++) {
        spiReceiveByte(SDC_SPI_REG, &token);
        if (token != 0xFF)
            break;
    }

    /* If not valid data token, return with error */
    if(token != dataToken) return false;

    /* Receive the data block into buffer */
    while (btr) {
        spiReceiveByte(SDC_SPI_REG, buff++);
        spiReceiveByte(SDC_SPI_REG, buff++);
        btr -= 2;
    }
    
    /* Discard CRC */
    unsigned char crc;
    spiReceiveByte(SDC_SPI_REG, &crc); 
    spiReceiveByte(SDC_SPI_REG, &crc);

    return true;
}

#if _READONLY == 0

/**
 * @brief Send a data packet to the SD card.
 * @param buff 512 byte buffer containing the data to send.
 * @param token Data/Stop token.
 * @return bool True if the packet was sent successfully, false otherwise.
 */
static bool sendDataBlock(const BYTE *buff, BYTE token) {
    if (!isCardReady()) return false;

    spiTransmitByte(SDC_SPI_REG, token); // Send token

    if (token != SD_STOP_TRANSMISSION) { 
        for (unsigned int wc = 0; wc < SD_SECTOR_SIZE; wc++) { 
            // Send the data block
            spiTransmitByte(SDC_SPI_REG, *buff++);
        }
        
        // Send dummy CRC
        spiTransmitByte(SDC_SPI_REG, 0xFF);
        spiTransmitByte(SDC_SPI_REG, 0xFF);
        
        BYTE resp;
        spiReceiveByte(SDC_SPI_REG, &resp); /* Receive data response */
        if ((resp & SD_DATA_RESPONSE_MASK) != SD_DATA_RESPONSE_ACCEPTED) return false;
    }

    return true;
}

#endif /* _READONLY */

/**
 * @brief Send a command packet to the SD card.
 * @param cmd Command byte.
 * @param arg Argument.
 * @return BYTE Response byte.
 */
static BYTE sendCMD(BYTE cmd, DWORD arg) {
    const uint8_t resetCmdCrc = 0x95U; // CRC for SDC_CMD0
    const uint8_t checkVoltageCmdCrc = 0x87U; // CRC for SDC_CMD8
    
    if (!isCardReady()) return 0xFFU;

    /* Send command packet */
    spiTransmitByte(SDC_SPI_REG, cmd);                      /* Command */
    spiTransmitByte(SDC_SPI_REG, (BYTE)(arg >> 24));        /* Argument[31..24] */
    spiTransmitByte(SDC_SPI_REG, (BYTE)(arg >> 16));        /* Argument[23..16] */
    spiTransmitByte(SDC_SPI_REG, (BYTE)(arg >> 8));         /* Argument[15..8] */
    spiTransmitByte(SDC_SPI_REG, (BYTE)arg);                /* Argument[7..0] */

    /* Some commands require a CRC to be sent */
    BYTE crc = 0xFFU;
    if (cmd == SDC_CMD0) crc = resetCmdCrc;     
    else if (cmd == SDC_CMD8) crc = checkVoltageCmdCrc;
    spiTransmitByte(SDC_SPI_REG, crc);

    /* Skip a byte after "stop reading" cmd is sent */
    unsigned char tmp;
    if (cmd == SDC_CMD12) spiReceiveByte(SDC_SPI_REG, &tmp);
    
    /* Receive command response */
    BYTE res;
    const uint8_t maxTries = 10U, cmdRespMask = 0x80U; 
    for (uint8_t i = 0; i < maxTries; i++) {
        spiReceiveByte(SDC_SPI_REG, &res);
        if (!(res & cmdRespMask)) break;
    }

    return res;
}

/**
 * @brief Send SDC_CMD12 to the SD card to stop a multi-block read.
 * 
 * @return BYTE Response byte.
 */
static BYTE stopTransmission(void) {
    /* Send command packet - the argument for SDC_CMD12 is ignored. */
    spiTransmitByte(SDC_SPI_REG, SDC_CMD12);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);
    spiTransmitByte(SDC_SPI_REG, 0);

    /* Data transfer stops 2 bytes after 6-byte SDC_CMD12 */
    BYTE val;
    spiReceiveByte(SDC_SPI_REG, &val); spiReceiveByte(SDC_SPI_REG, &val);

    /* SDC should now send 2-6 0xFF bytes, the response byte, and then another 0xFF */
    /* Some cards don't send the 2-6 0xFF bytes */
    BYTE res;
    const uint8_t numBytesRcv = 8U;
    for(unsigned int n = 0; n < numBytesRcv; n++) {
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
    if (drv) return STA_NOINIT;            /* Supports only single drive */
    if (stat & STA_NODISK) return stat;    /* No card in the socket */

    turnOnSDC();

    assertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);
    BYTE ty = 0;

    const uint8_t maxTries = 100U;
    for (uint8_t i = 0; i < maxTries; i++) {
        if (sendCMD(SDC_CMD0, 0) == 1U) break; /* Put the SD card into SPI mode*/
    }
    
    // Even if SDC_CMD0 fails, we'll try to continue.
    if (sendCMD(SDC_CMD8, 0x1AA) == 1U) {    
        /* SDC Ver2+ */
        const uint8_t ocrSize = 4U;
        BYTE ocr[ocrSize];
        for (unsigned int i = 0; i < ocrSize; i++) spiReceiveByte(SDC_SPI_REG, &ocr[i]);
        
        if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
            /* The card can work at vdd range of 2.7-3.6V */
            for (uint8_t i = 0; i < maxTries; i++) {
                if (sendCMD(SDC_CMD55, 0) <= 1 && sendCMD(SDC_CMD41, 1UL << 30) == 0) {
                    /* SDC_CMD41 with HCS bit */
                    if (sendCMD(SDC_CMD58, 0) == 0) {    /* Check CCS bit */
                        for (unsigned int i = 0; i < ocrSize; i++) spiReceiveByte(SDC_SPI_REG, &ocr[i]);
                        ty = (ocr[0] & 0x40) ? 6 : 2;
                    }
                }
            }
        }
    } else {                            
        /* SDC Ver1 or MMC */
        ty = (sendCMD(SDC_CMD55, 0) <= 1 && sendCMD(SDC_CMD41, 0) <= 1) ? 2 : 1;    /* SDC : MMC */
        for (unsigned int i = 0; i < maxTries; i++) {
            if (ty == 2) {
                if (sendCMD(SDC_CMD55, 0) <= 1 && sendCMD(SDC_CMD41, 0) == 0) break;    /* SDC_CMD41 */
            } else {
                if (sendCMD(SDC_CMD1, 0) == 0) break;                                /* SDC_CMD1 */
            }

            if ((i == maxTries - 1) || sendCMD(SDC_CMD16, 512) != 0) {   /* Select R/W block length */    
                ty = 0;
            }
        }
    }

    cardType = ty;
    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

    unsigned char tmp;
    spiReceiveByte(SDC_SPI_REG, &tmp);            /* Idle (Release DO) */

    if (ty) {            
        /* Initialization succeded */
        stat &= ~STA_NOINIT;    // Clear STA_NOINIT
    } else {            
        /* Initialization failed */
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
    if (count == 1) {    
        /* Single block read */
        if ((sendCMD(SDC_CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
            && rcvDataBlock(buff, 512))
            count = 0;
    }
    else {                
        /* Multiple block read */
        if (sendCMD(SDC_CMD18, sector) == 0) {    /* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvDataBlock(buff, 512)) break;
                buff += 512;
            } while (--count);
            stopTransmission();                /* STOP_TRANSMISSION */
        }
    }

    deassertChipSelect(SDC_SPI_PORT, SDC_SPI_CS);

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
        if ((sendCMD(SDC_CMD24, sector) == 0)    /* WRITE_BLOCK */
            && sendDataBlock(buff, 0xFE))
            count = 0;
    }
    else {                /* Multiple block write */
        if (cardType & 2) {
            sendCMD(SDC_CMD55, 0); sendCMD(SDC_CMD23, count);    /* SDC_CMD23 */
        }
        if (sendCMD(SDC_CMD25, sector) == 0) {    /* WRITE_MULTIPLE_BLOCK */
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
            case 0:        
                /* Sub control code == 0 (turn off power) */
                if (checkPower() == POWER_ON)
                    turnOffSDC();
                res = RES_OK;
                break;
            case 1:        
                /* Sub control code == 1 (turn on power) */
                turnOnSDC();
                res = RES_OK;
                break;
            case 2:        
                /* Sub control code == 2 (get power status) */
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
                if ((sendCMD(SDC_CMD9, 0) == 0) && rcvDataBlock(csd, 16)) {
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
                if (isCardReady())
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