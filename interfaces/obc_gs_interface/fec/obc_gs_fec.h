#pragma once

#include "obc_gs_errors.h"
#include "correct.h"

#include <stdint.h>

#define RS_DECODED_SIZE 223U
#define RS_ENCODED_SIZE 255U
#define PACKED_TELEM_PACKET_SIZE RS_DECODED_SIZE

typedef struct {
  uint8_t data[RS_ENCODED_SIZE];
} packed_rs_packet_t;

typedef struct {
  uint8_t data[PACKED_TELEM_PACKET_SIZE];
} packed_telem_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief takes in a packed telemtry array and encodes it using reed solomon
 *
 * @param telemData packed telemtry data array that needs to be encoded
 * @param rsData 255 byte array with the reed solomon encoded data
 */
obc_gs_error_code_t rsEncode(uint8_t *telemData, packed_rs_packet_t *rsData);

/**
 * @brief Decodes the reed solomon data
 *
 * @param rsData 255 byte array that has encoded reed solomon data
 * @param decodedData pointer to a uint8_t array of size 223B
 * @param decodedDataLen length of the decodedData array
 */
obc_gs_error_code_t rsDecode(packed_rs_packet_t *rsData, uint8_t *decodedData, uint8_t decodedDataLen);

/**
 * @brief initializes the rs variable to be used for rs encryption and decryption
 */
void initRs(void);

/**
 * @brief cleans up the memory allocated for the rs variable
 */
void destroyRs(void);

#ifdef __cplusplus
}
#endif
