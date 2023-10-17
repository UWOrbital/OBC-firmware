#include "test_fram.h"
#include "fm25v20a.h"
#include "obc_print.h"


void testFRAM(void){

    obc_error_code_t errCode;
    uint8_t id[9] = {0};
    uint8_t testId[9] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xC2, 0x25, 0x08};

    errCode = framReadID(&id, sizeof(id));

    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM Read ID unsuccessful. Error code: %d\r\n", errCode);
    }    
    else if (memcmp(id, testId, 9) == 0){
        sciPrintf("SUCCESS: Queried ID matches expected ID");
    }
    else{
        sciPrintf("ERROR - Invalid ID read\r\n");
    }
    
    
    uint8_t readData;

    errCode = framRead(0x12345, readData, sizeof(readData));
    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM read operation unsuccessful. Error code: %d\r\n", errCode);
    }


    uint8_t writeData = 123;

    errCode = framWrite(0x12345, writeData, sizeof(writeData));
    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM write operation unsuccessful. Error code: %d\r\n", errCode);
    }


    uint8_t newReadData;

    errCode = framRead(0x12345, newReadData, sizeof(newReadData));
    if (errCode != OBC_ERR_CODE_SUCCESS){
        sciPrintf("ERROR - FRAM second read operation unsuccessful. Error code: %d\r\n", errCode);
    }

    if (newReadData == writeData){
        sciPrintf("SUCCESS: Read data matches written data\r\n");
    }
    else if (newReadData == readData){
        sciPrintf("ERROR: Data not updated after write operation\r\n");
    }
    else{
        sciPrintf("ERROR: Data corrupted\r\n");
    }


}
