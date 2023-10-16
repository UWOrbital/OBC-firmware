#include "test_fram.h"
#include "fm25v20a.h"
#include "obc_print.h"


void testFRAM(void){

    obc_error_code_t errCode;
    uint8_t read_data;
    uint8_t write_data = 12345;
    uint8_t new_read_data;
    uint8_t id[9] = {0};

    errCode = framReadID(&id, sizeof(id));

    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM Read ID unsuccessful. Error code: %d\r\n", errCode);
    }    
    else if (id[0] != 0x7F || id[1] != 0x7F || id[2] != 0x7F || id[3] != 0x7F || id[4] != 0x7F || id[5] != 0x7F || id[6] != 0xC2 || id[7] != 0x25 || id[8] != 0x08){
        sciPrintf("ERROR - Invalid ID read\r\n");
    }
    else{
        sciPrintf("SUCCESS: Queried ID matches expected ID");
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
        sciPrintf("ERROR - FRAM read operation unsuccessful. Error code: %d\r\n", errCode);
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