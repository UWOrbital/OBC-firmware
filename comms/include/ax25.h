#ifndef COMMS_INCLUDE_AX25_H_
#define COMMS_INCLUDE_AX25_H_

#include "obc_errors.h"
#include "fec.h"

#include <stdint.h>

#define AX25_START_FLAG_BYTES 1
#define AX25_END_FLAG_BYTES 1
#define AX25_TOTAL_FLAG_BYTES 2
#define AX25_SRC_ADDR_BYTES 7
#define AX25_DEST_ADDR_BYTES 7
#define AX25_ADDRESS_BYTES (AX25_SRC_ADDR_BYTES + AX25_DEST_ADDR_BYTES)
/* Number of control bytes currently used for our I and S Frames */
#define AX25_MOD128_CONTROL_BYTES 2
/* Number of control bytes currently used for our U Frames */
#define AX25_MOD8_CONTROL_BYTES 1
#define AX25_PID_BYTES 1
#define AX25_FCS_BYTES 2
#define AX25_INFO_BYTES 255
#define AX25_MINIMUM_I_FRAME_LEN                                                                              \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES + \
   AX25_INFO_BYTES)
/*
AX.25 bit stuffing will add a bit after every 5 consecutive 1s. The maximum number of added bits for a buffer of x bytes
is (8*x)/5. As a result, the maximum number of bytes in a frame is [(8*x) + (8*x)/5] / 8 = x * 6/5
*/
/* The maximum AX25 Frame length is also the maximum I Frame length since I frames are the largest type of frames */
#define AX25_MAXIMUM_PKT_LEN AX25_MINIMUM_I_FRAME_LEN * 6 / 5
#define AX25_SUPERVISORY_FRAME_LENGTH \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES)
#define AX25_MINIMUM_U_FRAME_CMD_LENGTH \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES)
/* same calculation as explained above for maximum bytes after bit stuffing */
#define AX25_MAXIMUM_U_FRAME_CMD_LENGTH                 \
  (AX25_MINIMUM_U_FRAME_CMD_LENGTH +                    \
   (AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES) * 6 / \
       5)  // Only place where a bit may get stuffed is in the control byte or address bytes

#define AX25_START_FLAG_POSITION 0
#define AX25_DEST_ADDR_POSITION AX25_START_FLAG_BYTES
#define AX25_SRC_ADDR_POSITION (AX25_DEST_ADDR_POSITION + AX25_DEST_ADDR_BYTES)
#define AX25_CONTROL_BYTES_POSITION (AX25_SRC_ADDR_POSITION + AX25_SRC_ADDR_BYTES)
/* Index of PID for a mod 8 frame (ex: U frame) which only has one control byte */
#define AX25_MOD8_PID_POSITION (AX25_CONTROL_BYTES_POSITION + AX25_MOD8_CONTROL_BYTES)
/* Index of PID for a mod 128 frame (ex: our I Frames or S frames) which have two control bytes */
#define AX25_MOD128_PID_POSITION (AX25_CONTROL_BYTES_POSITION + AX25_MOD128_CONTROL_BYTES)
#define AX25_INFO_FIELD_POSITION (AX25_MOD128_PID_POSITION + AX25_PID_BYTES)
#define AX25_I_FRAME_FCS_POSITION (AX25_INFO_FIELD_POSITION + AX25_INFO_BYTES)
#define AX25_S_FRAME_FCS_POSITION AX25_INFO_FIELD_POSITION
#define AX25_U_FRAME_FCS_POSITION (AX25_MOD8_PID_POSITION + AX25_PID_BYTES)

#define AX25_FLAG 0x7E
#define AX25_PID 0xF0U

#define AX25_S_FRAME_RR_CONTROL 0x01U
#define AX25_S_FRAME_RNR_CONTROL 0x05U
#define AX25_S_FRAME_REJ_CONTROL 0x09U
#define AX25_S_FRAME_SREJ_CONTROL 0x0DU

#define MAX_U_FRAME_CMD_VALUE 2

typedef struct {
  uint8_t data[AX25_MINIMUM_I_FRAME_LEN];
  uint16_t length;
} unstuffed_ax25_i_frame_t;

typedef struct {
  uint8_t data[AX25_MAXIMUM_PKT_LEN];
  uint16_t length;
} packed_ax25_i_frame_t;

typedef struct {
  uint8_t data[AX25_MAXIMUM_U_FRAME_CMD_LENGTH];
  uint8_t length;
} packed_ax25_u_frame_t;

typedef struct {
  uint8_t data[AX25_DEST_ADDR_BYTES];
  uint8_t length;
} ax25_addr_t;

typedef enum { U_FRAME_CMD_CONN, U_FRAME_CMD_DISC, U_FRAME_CMD_ACK } u_frame_cmd_t;

extern ax25_addr_t cubesatCallsign;
extern ax25_addr_t groundStationCallsign;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief adds ax.25 headers onto telemetry being downlinked and stores the length of the packet in az25Data->length
 *
 * @param telemData data to send that needs ax.25 headers added onto it
 * @param telemDataLen length of the telemData array
 * @param ax25Data array to store the ax.25 frame
 * @param destAddress address of the destination for the ax25 packet
 *
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully added
 */
obc_error_code_t ax25SendIFrame(uint8_t *telemData, uint8_t telemDataLen, packed_ax25_i_frame_t *ax25Data,
                                const ax25_addr_t *destAddress);

/**
 * @brief format a buffer into a U frame command such as connect, disconnect, or acknowledge
 *
 * @param ax25Data buffer to store the U frame to be sent
 * @param cmd the U frame command you want to send
 * @param pollFinalBit what to set the poll/final bit to in the U frame (either 1 or 0)
 * @param destAddress address of the destination for the ax25 packet
 *
 * @return obc_error_code_t - whether or not the buffer was correctly formatted
 */
obc_error_code_t ax25SendUFrame(packed_ax25_u_frame_t *ax25Data, uint8_t cmd, uint8_t pollFinalBit,
                                const ax25_addr_t *destAddress);

/**
 * @brief checks for a valid ax25 frame and performs command handling if necessary
 *
 * @param ax25Data the received ax.25 frame
 *
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
 */
obc_error_code_t ax25Recv(unstuffed_ax25_i_frame_t *unstuffedPacket);

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param packet pointer to a buffer with the received stuffed ax.25 data
 * @param packetLen length of the packetLen buffer
 * @param unstuffedPacket pointer to a buffer to hold the unstuffed ax.25 packet
 * @param unstuffedPacketLen stores the number of bytes written to unstuffedPacket
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
obc_error_code_t ax25Unstuff(uint8_t *packet, uint16_t packetLen, uint8_t *unstuffedPacket,
                             uint16_t *unstuffedPacketLen);

#ifdef __cplusplus
}
#endif

#endif /* COMMS_INCLUDE_AX25_H_ */
