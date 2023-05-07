#include "ax25.h"
#include "obc_logging.h"
#include <sys_common.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
/**
 * @brief adds ax.25 headers onto telemtry being downlinked
 * 
 * @param rsData reed solomon data that needs ax.25 headers added onto it
 * @param out array to store the ax.25 frame
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully added
*/
uint8_t FR_COUNT = 0;

obc_error_code_t ax25Send(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data) {
    if (rsData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (ax25Data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (FR_COUNT == 0){
        //Send connection request
        //Flip to receive mode
        //Confirm Ack and switch back to send mode
        //Send frame
    }

    if (FR_COUNT == 128){
        //Send DISC request
        //Flip to receive mode
        //Confirm Ack and switch back to send mode
        FR_COUNT = 0;
        //Send frame

    }
// Byte incrament variable
    int i = 0;

// Starting flag
    memset(ax25Data+i, FLAG, FLAG_LENGTH);
    i+= FLAG_LENGTH;

// Destination callsign
    memcpy(ax25Data+i, DEST_CALLSIGN, sizeof(DEST_CALLSIGN)) ;
    i += sizeof(DEST_CALLSIGN);
    if (sizeof(DEST_CALLSIGN) < ADDR_LEN){
    memset(ax25Data+i, SPACE_CHAR, (ADDR_LEN-sizeof(DEST_CALLSIGN)));
    i += (ADDR_LEN-sizeof(DEST_CALLSIGN));
    }

// Source callsign
    memcpy(ax25Data+i, SRC_CALLSIGN, sizeof(DEST_CALLSIGN)) ;
    i += sizeof(SRC_CALLSIGN);
    if (sizeof(SRC_CALLSIGN) < ADDR_LEN){
    memset(ax25Data+i, SPACE_CHAR, (ADDR_LEN-sizeof(SRC_CALLSIGN)));
    i += (ADDR_LEN-sizeof(SRC_CALLSIGN));
    }

// Control field
    uint8_t ctrl = FR_COUNT<<1;
    memset(ax25Data+i, ctrl, 1);
    i += 1;

// PID
    memset(ax25Data+i, PID_DEFAULT, 1);
    i += 1;

// Data
    memcpy(ax25Data+i, rsData, 255);
    i += 255;

// fcs message
    uint16_t fcs = ax25_fcs (ax25Data + FLAG_LENGTH, i - FLAG_LENGTH);
    ax25Data[i++] = fcs & 0xFF;
    ax25Data[i++] = (fcs >> 8) & 0xFF;

// Ending flag
    memset(ax25Data+i, FLAG, FLAG_LENGTH);
    i+=1;

// Stuff message
    uint8_t *stuffed_data;
    size_t stuffed_data_len;
    bit_stuffing(ax25Data, i,  &stuffed_data, &stuffed_data_len);
    ax25Data = stuffed_data;

// Incrament frame cout
    FR_COUNT+=1;

    return OBC_ERR_CODE_SUCCESS;
}

uint16_t ax25_fcs (uint8_t *buffer, size_t len){
  uint16_t fcs = 0xFFFF;
  while (len--) {
    fcs = (fcs >> 8) ^ crc16_ccitt_table_reverse[(fcs ^ *buffer++) & 0xFF];
  }
  return fcs ^ 0xFFFF;
}

void bit_stuffing(const uint8_t *data, size_t data_len, uint8_t **stuffed_data, size_t *stuffed_data_len) {
    size_t max_stuffed_len = data_len + (data_len / 5) + 1 + 2;
    *stuffed_data = (uint8_t *)malloc(max_stuffed_len);

    size_t i = 0, j = 0, bit_count = 0, one_count = 0;
    uint8_t current_bit;
    memset(*stuffed_data, FLAG, FLAG_LENGTH);
    for (i = FLAG_LENGTH*8; i < (data_len-FLAG_LENGTH) * 8; ++i) {
        current_bit = (data[i / 8] >> (7 - (i % 8))) & 1;
        (*stuffed_data)[j / 8 + 1] |= (current_bit << (7 - (j % 8)));
        j++;

        if (current_bit == 1) {
            one_count++;
            if (one_count == 5) {
                one_count = 0;
                j++;
            }
        } else {
            one_count = 0;
        }
    }
    memset(*stuffed_data + (j+7)/8 + 1, FLAG, FLAG_LENGTH);

    *stuffed_data_len = (j + 7) / 8 + 2;

}

/**
 * @brief strips away the ax.25 headers from a received packet
 * 
 * @param ax25Data the received ax.25 frame
 * @param rsData 255 byte array to store the reed solomon encoded data without ax.25 headers
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
*/
obc_error_code_t ax25Recv(packed_ax25_packet_t *ax25Data, packed_rs_packet_t *rsData){
    if (rsData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (ax25Data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Implement AX.25 frame stripping

    return OBC_ERR_CODE_SUCCESS;
}
