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
uint8_t FRAME_COUNT = 0;

obc_error_code_t ax25Send(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data) {
    if (rsData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (ax25Data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (FRAME_COUNT == 128){
        FRAME_COUNT = 0;
    }
// Byte incrament variable
    int OFFSET = 0;

// Temporary array before bit_stuffing
    uint8_t ax25Data_TEMP[274];

// Address setting
    set_address(SRC_CALLSIGN, DEST_CALLSIGN, 5, 4, 0x80, 0x00, ax25Data_TEMP, &OFFSET)

// Control field
    memset(ax25Data_TEMP+OFFSET, 0, 1);
    OFFSET += 1;

    uint8_t ctrl = FRAME_COUNT<<1;
    memset(ax25Data_TEMP+OFFSET, ctrl, 1);
    OFFSET += 1;

// PID
    memset(ax25Data_TEMP+OFFSET, PID_DEFAULT, 1);
    OFFSET += 1;

// Data
    memcpy(ax25Data_TEMP+OFFSET, rsData, 255);
    OFFSET += 255;

// fcs message
    uint16_t fcs = ax25_fcs (ax25Data + FLAG_LENGTH, OFFSET - FLAG_LENGTH);
    ax25Data_TEMP[OFFSET++] = fcs & 0xFF;
    ax25Data_TEMP[OFFSET++] = (fcs >> 8) & 0xFF;

// Stuff message and add flags
    bit_stuffing(ax25Data_TEMP, ax25Data);

// Incrament frame cout
    FRAME_COUNT+=1;

    return OBC_ERR_CODE_SUCCESS;
}

void set_address(uint8_t *SRC_CALLSIGN, uint8_t *DEST_CALLSIGN, int SRC_LEN, int DEST_LEN, uint8_t SRC_SSID, uint8_t DEST_SSID, uint8_t * ax25Data, int *OFFSET){
    uint8_t SPACE_CHAR = 0x40;

// Destination addressing
    memcpy(ax25Data+*OFFSET, DEST_CALLSIGN, DEST_LEN) ;
    *OFFSET += DEST_LEN;
    memset(ax25Data+*OFFSET, SPACE_CHAR, (6-DEST_LEN));
    *OFFSET += (6-DEST_LEN);
    memset(ax25Data+*OFFSET, DEST_SSID, 1);
    *OFFSET += 1;


// Source addressing
    memcpy(ax25Data+*OFFSET, SRC_CALLSIGN, DEST_LEN) ;
    *OFFSET += DEST_LEN;
    memset(ax25Data+*OFFSET, SPACE_CHAR, (6-DEST_LEN));
    *OFFSET += (6-DEST_LEN);
    memset(ax25Data+*OFFSET, SRC_SSID, 1);
    *OFFSET += 1;
}
uint16_t ax25_fcs (uint8_t *buffer, size_t len){
  uint16_t fcs = 0xFFFF;
  while (len--) {
    fcs = (fcs >> 8) ^ crc16_ccitt_table_reverse[(fcs ^ *buffer++) & 0xFF];
  }
  return fcs ^ 0xFFFF;
}

void bit_stuffing(uint8_t *RAW_DATA, uint8_t *STUFFED_DATA) {

    size_t RAW_OFFSET = 0, STUFFED_OFFSET = 8, bit_count = 0, one_count = 0;
    uint8_t current_bit;

// Set starting flag
    memset(stuffed_data, FLAG, 1);
// Cycle through raw data to find 1s
    for (RAW_OFFSET = 0; RAW_OFFSET < (274) * 8; ++RAW_OFFSET) {
        current_bit = (data[RAW_OFFSET / 8] >> (7 - (RAW_OFFSET % 8))) & 1;
        stuffed_data[STUFFED_OFFSET / 8] |= (current_bit << (7 - (STUFFED_OFFSET % 8)));
        STUFFED_OFFSET++;

        if (current_bit == 1) {
            one_count++;
            if (one_count == 5) {
                one_count = 0;
                stuffed_data[STUFFED_OFFSET / 8] |= (0 << (7 - (STUFFED_OFFSET % 8)));
                STUFFED_OFFSET++;
            }
        } else {
            one_count = 0;
        }
    }
// Set ending flag
    memset(stuffed_data + (STUFFED_OFFSET/8) + 1, FLAG, 1);
}



