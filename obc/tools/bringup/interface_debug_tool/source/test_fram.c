#include "test_fram.h"
// #include "obc_sci_io.h"
#include "fm25v20a.h"

    /*The user can verify that writes are disabled
by reading the WEL bit in the Status Register and verifying that
WEL is equal to ‘0’.  page 6*/

 /* One way to check if the fram is working is to querry if for it's ID. 
 It should return a specific series of numbers. You can also test if reads and writes work by doing a read, then a write,
  then a read and seeing if anything changed*/

obc_error_code_t testFRAM_ID(void){

    initFRAM();

    obc_error_code_t errCode;
    uint8_t id[9] = {0};
    
    errCode = framReadID(&id, sizeof(uint8_t) * 9);

    if (errCode != OBC_ERR_CODE_SUCCESS){
        return errCode;
    }
    else if (id[0] != 0x7F || id[1] != 0x7F || id[2] != 0x7F || id[3] != 0x7F || id[4] != 0x7F || id[5] != 0x7F || id[6] != 0xC2 || id[7] != 0x25 || id[8] != 0x08){
        return OBC_ERR_CODE_INVALID_STATE;  // Check ID
    }

    return OBC_ERR_CODE_SUCCESS;
}