#ifndef COMMS_INCLUDE_FEC_H_
#define COMMS_INCLUDE_FEC_H_

#include "obc_errors.h"
#include "aes128.h"
#include "correct.h"

#include <stdint.h>

#define REED_SOLOMON_DECODED_BYTES 223U
#define REED_SOLOMON_ENCODED_BYTES 255U
#define PACKED_TELEM_PACKET_SIZE REED_SOLOMON_DECODED_BYTES

extern correct_reed_solomon* rs;

typedef struct {
    uint8_t data[REED_SOLOMON_ENCODED_BYTES];
} packed_rs_packet_t;

typedef struct {
    uint8_t data[PACKED_TELEM_PACKET_SIZE];
} packed_telem_packet_t;

/**
 * @brief takes in a packed telemtry array and encodes it using reed solomon
 * 
 * @param telemData packed telemtry data array that needs to be encoded
 * @param rsData 255 byte array with the reed solomon encoded data
 * 
 * @return obc_error_code_t - whether or not the data was successfully encoded
*/
obc_error_code_t rsEncode(packed_telem_packet_t *telemData, packed_rs_packet_t *rsData);

/**
 * @brief decodes the reed solomon data and splits it into 2 128B AES blocks
 * 
 * @param rsData 255 byte array that has encoded reed solomon data
 * @param aesSerializedData pointer to an array of bytes to hold the decoded reed solomon data
 * @param aesSerializedDataLen length of the aesSerializedData array
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(packed_rs_packet_t *rsData, uint8_t *aesSerializedData, uint8_t aesSerializedDataLen);

/**
 * @brief initializes the rs variable to be used for rs encryption and decryption
*/
void initRs(void);

/**
 * @brief cleans up the memory allocated for the rs variable
*/
void destroyRs(void);
#endif /* COMMS_INCLUDE_FEC_H_ */
