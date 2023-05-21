#include "obc_sci_io.h"

#include "obc_spi_io.h"
#include "obc_sci_io.h"
#include "fram.h"
#include "obc_board_config.h"
#include "obc_persistent_data_config.h"
#include "obc_persistent_store.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define msg_buffer_size 50
static char msg[msg_buffer_size] = {0};
void dump_contents(uint32_t start,uint32_t end){

    uint8_t byte;
    sciPrintText("Dumping Contents\r\n", strlen("Dumping Contents\r\n"));
    for(uint32_t i=start;i<FRAM_MAX_ADDRESS && i < end; i++){
        framRead(i, &byte, 1);
        memset(msg, 0, msg_buffer_size);
        snprintf(msg, msg_buffer_size, "%05X : %02X\r\n", i, byte);
        sciPrintText((unsigned char *) msg, strlen(msg));
    }
}

void integrity_check(void){
    static fram_sys_data_t sys_test_data = {0};
    static fram_adcs_data_t adcs_test = {0};
    static fram_comms_data_t comms_test = {0};
    static fram_cdh_data_t cdh_test = {0};
    static fram_eps_data_t eps_test = {0};
    static fram_payload_data_t payload_test= {0};
    if(getPersistentSysData(&sys_test_data) != OBC_ERR_CODE_PERSISTENT_CORRUPTED){
        sciPrintText("sys passed\r\n", strlen("sys passed\r\n"));
    } else {
        sciPrintText("sys failed\r\n", strlen("sys failed\r\n"));
    }

    if(getPersistentADCSData(&adcs_test) != OBC_ERR_CODE_PERSISTENT_CORRUPTED){
        sciPrintText("adcs passed\r\n", strlen("adcs passed\r\n"));
    } else {
         sciPrintText("adcs failed\r\n", strlen("adcs failed\r\n"));
    }
    
    if(getPersistentCDHData(&cdh_test) != OBC_ERR_CODE_PERSISTENT_CORRUPTED){
        sciPrintText("cdh passed\r\n", strlen("cdh passed\r\n"));
    } else {
        sciPrintText("cdh failed\r\n", strlen("cdh failed\r\n"));
    }

    if(getPersistentCOMMSData(&comms_test) != OBC_ERR_CODE_PERSISTENT_CORRUPTED){
        sciPrintText("comms passed\r\n", strlen("comms passed\r\n"));
    } else {
        sciPrintText("comms failed\r\n", strlen("comms failed\r\n"));
    }

    if(getPersistentEPSData(&eps_test) != OBC_ERR_CODE_PERSISTENT_CORRUPTED){
        sciPrintText("eps passed\r\n", strlen("eps passed\r\n"));
    } else {
        sciPrintText("eps failed\r\n", strlen("eps failed\r\n"));
    }

    if(getPersistentPayloadData(&payload_test) != OBC_ERR_CODE_PERSISTENT_CORRUPTED){
        sciPrintText("payload passed\r\n", strlen("payload passed\r\n"));
    } else {
        sciPrintText("payload failed\r\n", strlen("payload failed\r\n"));
    }
}

int main(void) {
    // Initialize hardware.
    
    gioInit();
    sciInit();
    spiInit();

    
    // Initialize the SCI mutex.
    initSciMutex();
    initSpiMutex();
    uint8_t chipID[FRAM_ID_LEN];
    
   
  
    //Read Manufacture ID
    framReadID(chipID, FRAM_ID_LEN);
    snprintf(msg, msg_buffer_size, "ID:%X %X %X %X %X %X %X %X %X\r\n", chipID[0], chipID[1], chipID[2], chipID[3], chipID[4], chipID[5], chipID[6], chipID[7], chipID[8]);
    // Note: This will send through the USB port on the LaunchPad
    sciPrintText((unsigned char *) msg, strlen(msg));

    // Toggle the LED.
    gioToggleBit(SUPERVISOR_DEBUG_LED_GIO_PORT, SUPERVISOR_DEBUG_LED_GIO_BIT);
    
    //dump_contents(0x0, 0x100);
    sciPrintText("Writing test data\r\n", strlen("Writing test data\r\n"));
    fram_sys_data_t sys_test_data = {
        .unix_time = 1684701118,
        .obc_state = 1,
    };
    fram_adcs_data_t adcs_test = {
        .data1 = 1,
        .data2 = 2,
        .data3 = 3,
    };
    fram_comms_data_t comms_test = {
        .data1 = 4,
        .data2 = 5,
        .data3 = 6,
    };

    fram_cdh_data_t cdh_test = {
        .data1 = 7,
        .data2 = 8,
        .data3 = 9,
    };

    fram_eps_data_t eps_test = {
        .data1 = 10,
        .data2 = 11,
        .data3 = 12,
    };

    fram_payload_data_t payload_test= {
        .data1 = 13,
        .data2 = 14,
        .data3 = 15,
    };
    setPersistentSysData(sys_test_data);
    setPersistentADCSData(adcs_test);
    setPersistentCDHData(cdh_test);
    setPersistentCOMMSData(comms_test);
    setPersistentEPSData(eps_test);
    setPersistentPayloadData(payload_test);
    
    //dump_contents(0x0, 0x075);
    
    sciPrintText("\nIntegrity Check\r\n", strlen("\nIntegrity Check\r\n"));
    integrity_check();
    
    //dump_contents(0x0, 0x075);
    
    sciPrintText("\nTesting integrity check\r\n", strlen("\nTesting integrity check\r\n"));

    sciPrintText("Overwriting 1 byte sys data\r\n", strlen("Overwriting 1 byte sys data\r\n"));
    uint8_t corrupt = 0xFF;
    framWrite(0xC, &corrupt, 1);

    sciPrintText("Overwriting 1 byte cdh crc\r\n", strlen("Overwriting 1 byte cdh crc\r\n"));
    framWrite(0x3C, &corrupt, 1);

    sciPrintText("\nIntegrity Check\r\n", strlen("\nIntegrity Check\r\n"));
    integrity_check();


    return 0;
}