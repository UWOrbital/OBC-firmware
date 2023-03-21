#include "cc1120_decode.h"

#include "obc_logging.h"

obc_error_code_t ax25Recv(uint8_t *in, uint8_t *out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

obc_error_code_t rsDecode(uint8_t *in, aes_block_t *out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

obc_error_code_t aes128Decrypt(aes_block_t in, cmd_msg_t out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

obc_error_code_t tabulateCommands(cmd_msg_t command){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

void initDecodeDataTask(void){

}

static void vDecodeDataTask(void * pvParameters){

} 
