#include "cc1120_txrx.h"
#include "obc_logging.h"
#include "cc1120_mcu.h"
#include "cc1120_spi.h"
#include "cc1120_regs.h"


#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <FreeRTOSConfig.h>

#include <stdbool.h>


static SemaphoreHandle_t rxSemaphore = NULL;
static StaticSemaphore_t rxSemaphoreBuffer;
static SemaphoreHandle_t txSemaphore = NULL;
static StaticSemaphore_t txSemaphoreBuffer;

static const register_setting_t cc1120SettingsStd[] = {
    {CC1120_REGS_IOCFG3, 0xB0U},
    {CC1120_REGS_IOCFG2, 0x06U},
    {CC1120_REGS_IOCFG1, 0x03U},
    {CC1120_REGS_IOCFG0, 0x01U},
    {CC1120_REGS_SYNC_CFG0, 0x09U},
    {CC1120_REGS_SYNC_CFG1, 0x08U},
    {CC1120_REGS_SYNC0, 0x55U},
    {CC1120_REGS_SYNC1, 0x57U},
    {CC1120_REGS_DEVIATION_M, 0x3AU},
    {CC1120_REGS_MODCFG_DEV_E, 0x0AU},
    {CC1120_REGS_DCFILT_CFG, 0x1CU},
    {CC1120_REGS_PREAMBLE_CFG1, 0x18U},
    {CC1120_REGS_PREAMBLE_CFG0, 0x2AU},
    {CC1120_REGS_SYNC_CFG1, 0x17U},
    {CC1120_REGS_IQIC, 0xC6U},
    {CC1120_REGS_CHAN_BW, 0x08U},
    {CC1120_REGS_MDMCFG0, 0x05U},
    {CC1120_REGS_SYMBOL_RATE2, 0x73U},
    {CC1120_REGS_AGC_REF, 0x20U},
    {CC1120_REGS_AGC_CS_THR, 0x19U},
    {CC1120_REGS_AGC_CFG2, 0x20U},
    {CC1120_REGS_AGC_CFG1, 0xA9U},
    {CC1120_REGS_AGC_CFG0, 0xCFU},
    {CC1120_REGS_FIFO_CFG, 0x00U},
    {CC1120_REGS_FS_CFG, 0x14U},
    {CC1120_REGS_PKT_CFG0, 0x00U},
    {CC1120_REGS_PA_CFG0, 0x7DU},
    {CC1120_REGS_PKT_LEN, 0x0CU}};

static const register_setting_t cc1120SettingsExt[] = {
    {CC1120_REGS_EXT_IF_MIX_CFG, 0x00U},
    {CC1120_REGS_EXT_FREQOFF_CFG, 0x34U},
    {CC1120_REGS_EXT_FREQ2, 0x6CU},
    {CC1120_REGS_EXT_FREQ1, 0x7AU},
    {CC1120_REGS_EXT_FREQ0, 0xE1U},
    {CC1120_REGS_EXT_FS_DIG1, 0x00U},
    {CC1120_REGS_EXT_FS_DIG0, 0x5FU},
    {CC1120_REGS_EXT_FS_CAL1, 0x40U},
    {CC1120_REGS_EXT_FS_CAL0, 0x0EU},
    {CC1120_REGS_EXT_FS_DIVTWO, 0x03U},
    {CC1120_REGS_EXT_FS_DSM0, 0x33U},
    {CC1120_REGS_EXT_FS_DVC0, 0x17U},
    {CC1120_REGS_EXT_FS_PFD, 0x50U},
    {CC1120_REGS_EXT_FS_PRE, 0x6EU},
    {CC1120_REGS_EXT_FS_REG_DIV_CML, 0x14U},
    {CC1120_REGS_EXT_FS_SPARE, 0xACU},
    {CC1120_REGS_EXT_FS_VCO0, 0xB4U},
    {CC1120_REGS_EXT_XOSC5, 0x0EU},
    {CC1120_REGS_EXT_XOSC1, 0x03U},
    {CC1120_REGS_EXT_TOC_CFG, 0x89U}};

void initTxRxSemaphores(void){
    if(txSemaphore == NULL) {
        txSemaphore = xSemaphoreCreateBinaryStatic(&txSemaphoreBuffer);
    }
    if(rxSemaphore == NULL) {
        rxSemaphore = xSemaphoreCreateBinaryStatic(&rxSemaphoreBuffer);
    }
}

/**
 * @brief Gets the number of packets queued in the TX FIFO
 *
 * @param numPackets - A pointer to an 8-bit integer to store the number of packets in
 * @return obc_error_code_t - Whether or not the registe read was successful
 */
obc_error_code_t cc1120_get_packets_in_tx_fifo(uint8_t *numPackets)
{
    if(numPackets == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }
    return cc1120_read_ext_addr_spi(CC1120_REGS_EXT_NUM_TXBYTES, numPackets, 1);
}

/**
 * @brief Gets the state of the CC1120 from the MARCSTATE register
 *
 * @param stateNum - A pointer to an 8-bit integer to store the state in
 * @return obc_error_code_t - Whether or not the register read was successful
 */
obc_error_code_t cc1120_get_state(uint8_t *stateNum)
{
    if(stateNum == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }
    obc_error_code_t errCode = cc1120_read_ext_addr_spi(CC1120_REGS_EXT_MARCSTATE, stateNum, 1);
    *stateNum &= 0b11111;
    return errCode;
}

/**
 * @brief Resets CC1120 & initializes transmit mode
 *
 * @return obc_error_code_t - Whether or not the setup was a success
 */
obc_error_code_t cc1120_init(void)
{
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

    for (uint8_t i = 0; i < sizeof(cc1120SettingsStd) / sizeof(register_setting_t); i++)
    {
        RETURN_IF_ERROR_CODE(cc1120_write_spi(cc1120SettingsStd[i].addr, &cc1120SettingsStd[i].val, 1));        
    }

    for (uint8_t i = 0; i < sizeof(cc1120SettingsExt) / sizeof(register_setting_t); i++)
    {
        RETURN_IF_ERROR_CODE(cc1120_write_ext_addr_spi(cc1120SettingsExt[i].addr, &cc1120SettingsExt[i].val, 1));
    }

    return cc1120_strobe_spi(CC1120_STROBE_SFSTXON);
}

/**
 * @brief Adds the given data to the CC1120 FIFO buffer and transmits
 *
 * @param data - The packet to transmit
 * @param len - The size of the provided packet in bytes
 * @return obc_error_code_t
 */
obc_error_code_t cc1120_send(uint8_t *data, uint32_t len)
{
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

    if (len < 1)
    {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    bool largePacketFlag = false;

    // See section 8.1.5
    if (len > CC1120_MAX_PACKET_LEN)
    {
        // Temporarily set packet size to infinite
        uint8_t temp = 0x40;
        RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_CFG0, &temp, 1));
        
        // Set packet length to mod(len, 256) so that the correct number of bits
        // are sent when fixed packet mode gets reactivated
        temp = len % 100;
        RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_LEN, &temp, 1));
        
        largePacketFlag = true;
    }
    else
    { // If packet size < 255, use variable packet length mode
        // Set to variable packet length mode
        uint8_t temp = 0x20;
        RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_CFG0, &temp, 1));
        
        // Set max packet size
        temp = CC1120_MAX_PACKET_LEN;
        RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_LEN, &temp, 1));
        
        // Write current packet size
        uint8_t variableDataLen = (uint8_t)len;
        RETURN_IF_ERROR_CODE(cc1120_write_fifo(&variableDataLen, 1)); // Write packet size
    }

    // Write packet
    RETURN_IF_ERROR_CODE(cc1120_write_fifo(data, min(CC1120_TX_FIFO_SIZE, len)));
    
    RETURN_IF_ERROR_CODE(cc1120_strobe_spi(CC1120_STROBE_STX));
    
    uint32_t i;
    for (i = 0; i < (len - CC1120_TX_FIFO_SIZE) / 100; i++)
    {
        if (xSemaphoreTake(txSemaphore, portMAX_DELAY) == pdTRUE){
            RETURN_IF_ERROR_CODE(cc1120_write_fifo(data + CC1120_TX_FIFO_SIZE + i*100, 100));
        }
        RETURN_IF_ERROR_CODE(cc1120_strobe_spi(CC1120_STROBE_STX));
    }
    if (largePacketFlag){
        uint8_t temp = 0;
        RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_CFG0, &temp, 1));
    }

    if (len > 99){
        if (xSemaphoreTake(txSemaphore, portMAX_DELAY) == pdTRUE){
            RETURN_IF_ERROR_CODE(cc1120_write_fifo(data + CC1120_TX_FIFO_SIZE + i*100, len - CC1120_TX_FIFO_SIZE - 100*i));
        }
        RETURN_IF_ERROR_CODE(cc1120_strobe_spi(CC1120_STROBE_STX));
    }

    return errCode;
}

/* RX functions */

/**
 * @brief Gets the number of packets queued in the RX FIFO
 *
 * @param numPackets - A pointer to an 8-bit integer to store the number of packets in
 * @return obc_error_code_t - Whether or not the register read was successful
 */
obc_error_code_t cc1120_get_packets_in_rx_fifo(uint8_t *numPackets)
{
    return cc1120_read_ext_addr_spi(CC1120_REGS_EXT_NUM_RXBYTES, numPackets, 1);
}

/**
 * @brief Switches the cc1120 to RX mode to receive 278 bytes
 *
 * @param data - an array of 8-bit data with size of atleast 278 where received data is stored
 * @return obc_error_code_t
 */
obc_error_code_t cc1120_receive(uint8_t data[])
{
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

    // Temporarily set packet size to infinite
    uint8_t temp = 0x40;
    RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_CFG0, &temp, 1));
    
    // Set packet length to 78 so that the correct number of bits
    // are received when fixed packet mode gets reactivated after receiving 200 bytes
    temp = 78;

    RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_LEN, &temp, 1));
    
    RETURN_IF_ERROR_CODE(cc1120_strobe_spi(CC1120_STROBE_SRX));
    
    if(rxSemaphore != NULL) {
        for (int i = 0; i < 2; ++i){
            if(xSemaphoreTake(rxSemaphore, portMAX_DELAY) == pdTRUE){
                RETURN_IF_ERROR_CODE(cc1120_read_fifo(data + 100*i, 100));
            }
        }
        // Set to variable packet length mode
        temp = 0x20;
        RETURN_IF_ERROR_CODE(cc1120_write_spi(CC1120_REGS_PKT_CFG0, &temp, 1));
        
        if(xSemaphoreTake(rxSemaphore, portMAX_DELAY) == pdTRUE){
            RETURN_IF_ERROR_CODE(cc1120_read_fifo(data + 200, 78));
        }
    }
    return errCode;
}

SemaphoreHandle_t getRxSemaphore(void){
    return rxSemaphore;
}

SemaphoreHandle_t getTxSemaphore(void){
    return txSemaphore;
}