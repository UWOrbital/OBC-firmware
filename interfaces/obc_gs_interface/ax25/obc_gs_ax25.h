#pragma once

#include "obc_gs_errors.h"

#include <stdint.h>

#define AX25_START_FLAG_BYTES 1
#define AX25_END_FLAG_BYTES 1
#define AX25_TOTAL_FLAG_BYTES 2
#define AX25_SHARE_FLAG_BYTES 1
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
#define AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG                                                                   \
  (AX25_SHARE_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES + \
   AX25_INFO_BYTES)

// NOTE: We do MOD8 implementation
#define AX25_MINIMUM_I_FRAME_LEN                                                                            \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES + \
   AX25_INFO_BYTES)

// NOTE: This is for the mod128 implementation
/* #define AX25_MINIMUM_I_FRAME_LEN                                                                            \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES + \
   AX25_INFO_BYTES) */
/*
AX.25 bit stuffing will add a bit after every 5 consecutive 1s. The maximum number of added bits for a buffer of x bytes
is (8*x)/5. As a result, the maximum number of bytes in a frame is [(8*x) + (8*x)/5] / 8 = x * 6/5
*/
/* The maximum AX25 Frame length is also the maximum I Frame length since I frames are the largest type of frames */
#define AX25_MAXIMUM_PKT_LEN AX25_MINIMUM_I_FRAME_LEN * 6 / 5
#define AX25_SUPERVISORY_FRAME_LENGTH \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES + AX25_FCS_BYTES)
#define AX25_MINIMUM_U_FRAME_CMD_LENGTH \
  (AX25_TOTAL_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES + AX25_FCS_BYTES)
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

// NOTE: This is with the MOD8 implementation
#define AX25_INFO_FIELD_POSITION (AX25_MOD8_PID_POSITION + AX25_PID_BYTES)
// NOTE: This is for the mod128 implementation
// #define AX25_INFO_FIELD_POSITION (AX25_MOD128_PID_POSITION + AX25_PID_BYTES)
#define AX25_I_FRAME_FCS_POSITION (AX25_INFO_FIELD_POSITION + AX25_INFO_BYTES)
#define AX25_S_FRAME_FCS_POSITION AX25_INFO_FIELD_POSITION
#define AX25_U_FRAME_FCS_POSITION (AX25_MOD8_PID_POSITION)

#define AX25_FLAG 0x7E
#define AX25_PID 0xF0U

#define AX25_S_FRAME_RR_CONTROL 0x01U
#define AX25_S_FRAME_RNR_CONTROL 0x05U
#define AX25_S_FRAME_REJ_CONTROL 0x09U
#define AX25_S_FRAME_SREJ_CONTROL 0x0DU

#define MAX_U_FRAME_CMD_VALUE 3

#define CALL_SIGN_BYTES 6

#define CUBE_SAT_CALLSIGN (uint8_t *)"AKITO"
#define GROUND_STATION_CALLSIGN (uint8_t *)"ATLAS"
#define CALLSIGN_LENGTH 5
#define DEFAULT_SSID 0
#define DEFAULT_CONTROL_BIT 0
#define MAX_CONTINUOUS_PACKETS_ALLOWED 7

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

typedef enum { U_FRAME_CMD_CONN = 1, U_FRAME_CMD_DISC = 2, U_FRAME_CMD_ACK = 3 } u_frame_cmd_t;

extern ax25_addr_t cubesatCallsign;
extern ax25_addr_t groundStationCallsign;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief returns the number of unstuffed flag share bytes required depending on information length
 *
 * @param infoBytesLen length of the information data array
 */
static inline uint32_t ax25UnstuffedWithFlagShareLen(uint32_t infoBytesLen) {
  return (((infoBytesLen + AX25_INFO_BYTES - 1) / AX25_INFO_BYTES) * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + 1;
}

/**
 * @brief returns the number of stuffed flag share bytes required depending on information length
 *
 * @param infoBytesLen length of the information data array
 */
static inline uint32_t ax25StuffedWithFlagShareLen(uint32_t infoBytesLen) {
  return (((infoBytesLen + AX25_INFO_BYTES - 1) / AX25_INFO_BYTES) * (AX25_MAXIMUM_PKT_LEN - 1)) + 1;
}

/**
 * @brief prepares ax25data with appropriate number of I frames when utilizing flag-sharing
 *
 * @param telemData data to send that needs ax.25 headers added onto it
 * @param telemDataLen length of the telemData array
 * @param ax25Data array to store the ax.25 frame
 * @param ax25DataLen ax.25 array length
 * @param destAddress address of the destination for the ax25 packet
 */
obc_gs_error_code_t ax25SendIFrameWithFlagSharing(uint8_t *telemData, uint32_t telemDataLen, uint8_t *ax25Data,
                                                  uint32_t ax25DataLen, const ax25_addr_t *destAddress);

/**
 * @brief adds ax.25 headers onto telemetry being downlinked and stores the length of the packet in az25Data->length
 *
 * @param telemData data to send that needs ax.25 headers added onto it
 * @param telemDataLen length of the telemData array
 * @param ax25Data array to store the ax.25 frame
 */
obc_gs_error_code_t ax25SendIFrame(uint8_t *telemData, uint8_t telemDataLen, unstuffed_ax25_i_frame_t *ax25Data);

/**
 * @brief format a buffer into a U frame command such as connect, disconnect, or acknowledge
 *
 * @param ax25Data buffer to store the U frame to be sent
 * @param cmd the U frame command you want to send
 * @param pollFinalBit what to set the poll/final bit to in the U frame (either 1 or 0)
 *
 * @return obc_gs_error_code_t - whether or not the buffer was correctly formatted
 */
obc_gs_error_code_t ax25SendUFrame(packed_ax25_u_frame_t *ax25Data, uint8_t cmd, uint8_t pollFinalBit);

/**
 * @brief checks for a valid ax25 frame and performs command handling if necessary
 *
 * @param unstuffedPacket the received unstuffed ax.25 frame
 * @param uFrameCmd buffer to store the received command if the frame was a U frame
 * @return obc_gs_error_code_t - whether or not the ax.25 headers were successfully stripped
 */
obc_gs_error_code_t ax25Recv(unstuffed_ax25_i_frame_t *unstuffedPacket, u_frame_cmd_t *command);

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param packet pointer to a buffer with the received stuffed ax.25 data
 * @param packetLen length of the packetLen buffer
 * @param unstuffedPacket pointer to a buffer to hold the unstuffed ax.25 packet
 * @param unstuffedPacketLen stores the number of bytes written to unstuffedPacket
 */
obc_gs_error_code_t ax25Unstuff(uint8_t *packet, uint16_t packetLen, uint8_t *unstuffedPacket,
                                uint16_t *unstuffedPacketLen);

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param rawData unstuffed data buffer
 * @param rawDataLen length of the rawData buffer
 * @param stuffedData buffer to store the stuffed data
 * @param stuffedDataLen number of bytes filled into the stuffedData buffer
 */
obc_gs_error_code_t ax25Stuff(uint8_t *rawData, uint16_t rawDataLen, uint8_t *stuffedData, uint16_t *stuffedDataLen);

/**
 * @brief generates the destination address for the ax25 protocol
 *
 * @param address the address of the source
 * @param callSign the callsign for the address
 * @param callSignLength the length/number of bytes of the callSign
 * @param ssid the ssid of the address
 * @param controlBit the command/response bit in the ax25 protocol
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if an address was generated and error code if not
 */
obc_gs_error_code_t ax25GetDestAddress(ax25_addr_t *address, uint8_t callSign[], uint8_t callSignLength, uint8_t ssid,
                                       uint8_t controlBit);

/**
 * @brief generates the source address for the ax25 protocol
 *
 * @param address the address of the source
 * @param callSign the callsign for the address
 * @param callSignLength the length/number of bytes of the callSign
 * @param ssid the ssid of the address
 * @param controlBit the command/response bit in the ax25 protocol
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if an address was generated and error code if not
 */
obc_gs_error_code_t ax25GetSourceAddress(ax25_addr_t *address, uint8_t callSign[], uint8_t callSignLength, uint8_t ssid,
                                         uint8_t controlBit);

/**
 * @brief sets the address via Call Sign for the TNC we are communicating with in the current link session
 *
 * @param destAdress address of the TNC for the current link session
 */
void setCurrentLinkDestCallSign(uint8_t *destCallSign, uint8_t destCallSignLength, uint8_t ssid);

/**
 * @brief clears the destination address for the current link once we have disconnected
 */
void clearCurrentLinkDestAddress(void);

#ifdef __cplusplus
}
#endif
