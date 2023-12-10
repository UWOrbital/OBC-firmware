#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include "obc_rs.h"

#include <stdint.h>

obc_gs_error_code_t rsEncode(uint8_t *msg, packed_rs_packet_t *encoded) {
  rs_encode((void *)msg, (void *)encoded->data);
  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t rsDecode(packed_rs_packet_t *rsData, uint8_t *decodedData, uint8_t decodedDataLen) {
  int ret = rs_decode((void *)rsData->data, (void *)decodedData);

  return ret == 0 ? OBC_GS_ERR_CODE_SUCCESS : OBC_GS_ERR_CODE_REED_SOL_DEC_ERR;
}
