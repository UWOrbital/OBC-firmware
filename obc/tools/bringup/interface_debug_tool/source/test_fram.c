#include "test_fram.h"
#include "fm25v20a.h"
#include "obc_print.h"


void testFRAM(void){

    obc_error_code_t errCode;
    uint8_t read_data;
    uint8_t write_data = 12345;
    uint8_t new_read_data;
    uint8_t id[9] = {0};
    uint8_t test_id[9] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xC2, 0x25, 0x08};


    errCode = framReadID(&id, sizeof(uint8_t) * 9);

    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM Read ID unsuccessful. Error code: %d\r\n", errCode);
    }    
    else if (memcmp(id, test_id, 9) == 0){
        sciPrintf("SUCCESS: Queried ID matches expected ID");
    }
    else{
        sciPrintf("ERROR - Invalid ID read\r\n");
    }
    
    
    errCode = framRead(0x12345, read_data, sizeof(read_data));
    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM read operation unsuccessful. Error code: %d\r\n", errCode);
    }

    errCode = framWrite(0x12345, write_data, sizeof(write_data));
    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM write operation unsuccessful. Error code: %d\r\n", errCode);
    }

    errCode = framRead(0x12345, new_read_data, sizeof(new_read_data));
    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM second read operation unsuccessful. Error code: %d\r\n", errCode);
    }

    if (new_read_data == write_data){
        sciPrintf("SUCCESS: Read data matches written data\r\n");
    }
    else if (new_read_data == read_data){
        sciPrintf("ERROR: Data not updated after write operation\r\n");
    }
    else{
        sciPrintf("ERROR: Data corrupted\r\n");
    }


}