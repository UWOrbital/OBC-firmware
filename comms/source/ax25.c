#include "ax25.h"
#include "obc_logging.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define AX25_U_FRAME_SABME_CMD_CONTROL 0b01101111
#define AX25_U_FRAME_DISC_CMD_CONTROL 0b01000011
#define AX25_U_FRAME_ACK_CMD_CONTROL 0b01100011
#define POLL_FINAL_BIT_OFFSET 4

#ifndef SRC_CALLSIGN
#define SRC_CALLSIGN "ABCDEFG"
#endif

static uint8_t pktSentNum = 1;
static uint8_t pktReceiveNum = 1;

ax25_addr_t cubesatCallsign = {.data = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                               .length = AX25_DEST_ADDR_BYTES};  // mock cubesat address

ax25_addr_t groundStationCallsign = {.data = {0}, .length = AX25_DEST_ADDR_BYTES};  // Mock Ground station address

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param packet pointer to a buffer with the received stuffed ax.25 data
 * @param packetLen length of the packetLen buffer
 * @param unstuffedPacket pointer to a buffer to hold the unstuffed ax.25 packet
 * @param unstuffedPacketLen expected length of the unstuffed packet
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t ax25Unstuff(uint8_t *packet, uint16_t packetLen, uint8_t *unstuffedPacket,
                                    uint16_t unstuffedPacketLen);

/**
 * @brief strips away the ax.25 headers for an s Frame
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 * @param uplinkData 255 byte array to store the received data without ax.25 headers
 * @param recvAddress address of the receiver of the ax.25 packet
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t sFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket, uint8_t *uplinkData,
                                   ax25_addr_t *recvAddress);

/**
 * @brief strips away the ax.25 headers for an i Frame
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 * @param uplinkData 255 byte array to store the received data without ax.25 headers
 * @param recvAddress address of the receiver of the ax.25 packet
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t iFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket, uint8_t *uplinkData,
                                   ax25_addr_t *recvAddress);

/**
 * @brief calculates the FCS for an ax.25 packet
 *
 * @param data uint8_t array that holds the ax25 packet data
 * @param calculatedFcs pointer to a un16_t to hold the calculated FCS
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t fcsCalculate(const uint8_t *data, uint16_t *calculatedFcs);

/**
 * @brief checks if a received fcs is correct
 *
 * @param data the received ax.25 packet data
 * @param fcs the FCS of the received packet to be checked if it is valid or not
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was a valid fcs and error code if not
 */
static obc_error_code_t fcsCheck(const uint8_t *data, uint16_t fcs);

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param rawData unstuffed data buffer
 * @param rawDataLen length of the rawData buffer
 * @param stuffedData buffer to store the stuffed data
 * @param stuffedDataLen number of bytes filled into the stuffedData buffer
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t bitStuffing(uint8_t *rawData, uint16_t rawDataLen, uint8_t *stuffedData,
                                    uint16_t *stuffedDataLen);

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
                                ax25_addr_t *destAddress) {
  if (telemData == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (ax25Data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (destAddress == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (destAddress->length < AX25_DEST_ADDR_BYTES) {
    /* TODO: technically not an error, should be filled with spaces */
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  memset(ax25Data->data, 0, AX25_MAXIMUM_PKT_LEN);

  uint8_t ax25PacketUnstuffed[AX25_MINIMUM_I_FRAME_LEN] = {0};

  ax25PacketUnstuffed[0] = AX25_FLAG;
  // ax25PacketUnstuffed[AX25_MINIMUM_I_FRAME_LEN - 1] = AX25_FLAG;
  memcpy(ax25PacketUnstuffed + AX25_START_FLAG_BYTES, destAddress->data, AX25_DEST_ADDR_BYTES);
  uint8_t srcAddress[AX25_SRC_ADDR_BYTES] = SRC_CALLSIGN;
  memcpy(ax25PacketUnstuffed + AX25_START_FLAG_BYTES + AX25_DEST_ADDR_BYTES, srcAddress, AX25_SRC_ADDR_BYTES);
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES] = (pktReceiveNum << 1);
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + 1] = (pktSentNum << 1);
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES] = AX25_PID;
  memcpy(ax25PacketUnstuffed + AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES,
         telemData, telemDataLen);
  uint16_t fcs;
  RETURN_IF_ERROR_CODE(fcsCalculate(ax25PacketUnstuffed, &fcs));
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES +
                      AX25_INFO_BYTES] = (uint8_t)(fcs >> 8);
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_PID_BYTES +
                      AX25_INFO_BYTES + 1] = (uint8_t)(fcs & 0xFF);
  memset(ax25Data->data, 0, AX25_MAXIMUM_PKT_LEN);
  RETURN_IF_ERROR_CODE(bitStuffing(ax25PacketUnstuffed, AX25_MINIMUM_I_FRAME_LEN, ax25Data->data, &ax25Data->length));
  ax25Data->data[ax25Data->length - 1] = AX25_FLAG;
  ax25Data->data[0] = AX25_FLAG;
  pktSentNum++;
  return OBC_ERR_CODE_SUCCESS;
}

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
                                ax25_addr_t *destAddress) {
  if (cmd > MAX_U_FRAME_CMD_VALUE) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (ax25Data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (pollFinalBit > 1) {  // poll bit should be either 1 or 0
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;

  memset(ax25Data->data, 0, AX25_MAXIMUM_U_FRAME_CMD_LENGTH);

  uint8_t ax25PacketUnstuffed[AX25_MINIMUM_U_FRAME_CMD_LENGTH] = {0};

  ax25PacketUnstuffed[0] = AX25_FLAG;
  // ax25PacketUnstuffed[AX25_MINIMUM_I_FRAME_LEN - 1] = AX25_FLAG;
  memcpy(ax25PacketUnstuffed + AX25_START_FLAG_BYTES, destAddress->data, AX25_DEST_ADDR_BYTES);
  uint8_t srcAddress[AX25_SRC_ADDR_BYTES] = SRC_CALLSIGN;
  memcpy(ax25PacketUnstuffed + AX25_START_FLAG_BYTES + AX25_DEST_ADDR_BYTES, srcAddress, AX25_SRC_ADDR_BYTES);
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES] = pollFinalBit << POLL_FINAL_BIT_OFFSET;
  if (cmd == U_FRAME_CMD_ACK) {
    ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES] |= AX25_U_FRAME_ACK_CMD_CONTROL;
  } else if (cmd == U_FRAME_CMD_DISC) {
    ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES] |= AX25_U_FRAME_DISC_CMD_CONTROL;
  } else {
    ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES] |= AX25_U_FRAME_SABME_CMD_CONTROL;
  }

  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES] = AX25_PID;

  uint16_t fcs;
  RETURN_IF_ERROR_CODE(fcsCalculate(ax25PacketUnstuffed, &fcs));
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES + AX25_PID_BYTES] =
      (uint8_t)(fcs >> 8);
  ax25PacketUnstuffed[AX25_START_FLAG_BYTES + AX25_ADDRESS_BYTES + AX25_MOD8_CONTROL_BYTES + AX25_PID_BYTES + +1] =
      (uint8_t)(fcs & 0xFF);
  RETURN_IF_ERROR_CODE(
      bitStuffing(ax25PacketUnstuffed, AX25_MINIMUM_U_FRAME_CMD_LENGTH, ax25Data->data, (uint16_t *)&ax25Data->length));

  ax25Data->data[ax25Data->length - 1] = AX25_FLAG;
  ax25Data->data[0] = AX25_FLAG;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief strips away the ax.25 headers from a received packet
 *
 * @param ax25Data the received ax.25 frame
 * @param uplinkData 255 byte array to store the received data without ax.25 headers
 * @param recvAddress address of the receiver of the ax.25 packet
 * @param uplinkDataLen length of the uplinkData array
 *
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
 */
obc_error_code_t ax25Recv(packed_ax25_i_frame_t *ax25Data, uint8_t *uplinkData, uint8_t uplinkDataLen,
                          ax25_addr_t *recvAddress) {
  if (ax25Data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (uplinkData == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (uplinkDataLen < AX25_INFO_BYTES) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (ax25Data->length > AX25_MAXIMUM_PKT_LEN) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // check to make sure that the data starts and ends with a valid flag
  if ((ax25Data->data[0] != AX25_FLAG) || (ax25Data->data[ax25Data->length - 1] != AX25_FLAG)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  // perform bit unstuffing
  unstuffed_ax25_i_frame_t unstuffedPacket;
  RETURN_IF_ERROR_CODE(ax25Unstuff(ax25Data, &unstuffedPacket));

  bool supervisoryFrameFlag = false;
  if (unstuffedPacket.length == AX25_SUPERVISORY_FRAME_LENGTH) {
    /* TODO: not the best way to determine S flag? */
    supervisoryFrameFlag = true;
  } else if (unstuffedPacket.length != AX25_MINIMUM_I_FRAME_LEN) {
    /* TODO: same as above */
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Check FCS
  uint16_t fcs = unstuffedPacket.data[AX25_INFO_BYTES + AX25_PID_BYTES + AX25_MOD128_CONTROL_BYTES +
                                      AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES]
                 << 8;
  fcs |= unstuffedPacket.data[AX25_INFO_BYTES + AX25_PID_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_ADDRESS_BYTES +
                              AX25_START_FLAG_BYTES + 1];
  RETURN_IF_ERROR_CODE(fcsCheck(unstuffedPacket.data, fcs));
  if (supervisoryFrameFlag) {
    RETURN_IF_ERROR_CODE(sFrameRecv(&unstuffedPacket, uplinkData, recvAddress));
  } else {
    RETURN_IF_ERROR_CODE(iFrameRecv(&unstuffedPacket, uplinkData, recvAddress));
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param packet pointer to a buffer with the received stuffed ax.25 data
 * @param packetLen length of the packetLen buffer
 * @param unstuffedPacket pointer to a buffer to hold the unstuffed ax.25 packet
 * @param unstuffedPacketLen expected length of the unstuffed packet
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t ax25Unstuff(uint8_t *packet, uint16_t packetLen, uint8_t *unstuffedPacket,
                                    uint16_t unstuffedPacketLen) {
  uint8_t bitCount = 0;
  uint8_t stuffingFlag = 0;
  uint16_t unstuffedBitLength = 0;  // count as bits

  // Clear the unstuffed data
  memset(unstuffedPacket, 0, sizeof(unstuffedPacket->data));

  // Set the first flag
  unstuffedPacket[0] = AX25_FLAG;
  unstuffedBitLength += 8;

  // loop from second byte to second last byte since first and last are the flags
  for (uint16_t stuffedPacketIndex = 1; stuffedPacketIndex < packetLen - 1; ++stuffedPacketIndex) {
    uint8_t current_byte = packet->data[stuffedPacketIndex];

    for (uint8_t offset = 0; offset < 8; ++offset) {
      uint8_t bit = (current_byte >> (7 - offset)) & 0x01;

      if (stuffingFlag) {
        bitCount = 0;
        stuffingFlag = 0;
        continue;  // Skip adding the stuffed bit
      }

      if (bit == 1) {
        bitCount++;
        if (bitCount == 5) {
          bitCount = 0;
          stuffingFlag = 1;
        }
      } else {
        bitCount = 0;
      }
      if (unstuffedBitLength >= (unstuffedPacketLen - 1) * 8) {
        break;
      }
      unstuffedPacket[unstuffedBitLength / 8] |= bit << (7 - (unstuffedBitLength % 8));
      unstuffedBitLength++;
    }
  }
  // Add last flag to end of the packet, rounding up
  unstuffedPacket[(unstuffedBitLength + 7) / 8] = AX25_FLAG;

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief strips away the ax.25 headers for an s Frame
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 * @param uplinkData 255 byte array to store the received data without ax.25 headers
 * @param recvAddress address of the receiver of the ax.25 packet
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t sFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket, uint8_t *uplinkData,
                                   ax25_addr_t *recvAddress) {
  if (memcmp(unstuffedPacket->data + AX25_START_FLAG_BYTES, recvAddress->data, AX25_DEST_ADDR_BYTES) != 0) {
    return OBC_ERR_CODE_INVALID_AX25_PACKET;
  }
  uint8_t controlBytes[AX25_MOD128_CONTROL_BYTES] = {unstuffedPacket->data[AX25_ADDRESS_BYTES + 1],
                                                     unstuffedPacket->data[AX25_ADDRESS_BYTES + 2]};
  if (controlBytes[0] == AX25_S_FRAME_RR_CONTROL) {
    /* TODO: implement response for receieve ready S frame*/
  } else if (controlBytes[0] == AX25_S_FRAME_RNR_CONTROL) {
    /* TODO: implement response for receieve not ready S frame*/
  } else if (controlBytes[0] == AX25_S_FRAME_REJ_CONTROL) {
    /* TODO: implement response for rejected S frame*/
  } else if (controlBytes[0] == AX25_S_FRAME_SREJ_CONTROL) {
    /* TODO: implement response for selective reject S frame*/
  } else {
    return OBC_ERR_CODE_INVALID_AX25_PACKET;
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief strips away the ax.25 headers for an i Frame
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 * @param uplinkData 255 byte array to store the received data without ax.25 headers
 * @param recvAddress address of the receiver of the ax.25 packet
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t iFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket, uint8_t *uplinkData,
                                   ax25_addr_t *recvAddress) {
  if (memcmp(unstuffedPacket->data + AX25_START_FLAG_BYTES, recvAddress->data, AX25_DEST_ADDR_BYTES) != 0) {
    return OBC_ERR_CODE_INVALID_AX25_PACKET;
  }
  // first control byte will be the the after the flag and the address bytes
  // next control byte will be immediately after the previous one
  // See AX.25 standard
  uint8_t controlBytes[AX25_MOD128_CONTROL_BYTES] = {
      unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES],
      unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES + 1]};
  // LSB should be 0 for a valid I frame
  if (controlBytes[0] & 0x01) {
    return OBC_ERR_CODE_INVALID_AX25_PACKET;
  }
  if ((controlBytes[0] >> 1) != pktReceiveNum) {
    // TODO: implement retransmission requests
  }
  if (controlBytes[1] & 0x01) {
    // TODO: implement retransmissions
  }
  if ((controlBytes[1] >> 1) != pktSentNum) {
    // TODO: implement retransmissions
  }
  if (unstuffedPacket->data[AX25_MOD128_CONTROL_BYTES + AX25_ADDRESS_BYTES + 1] != AX25_PID) {
    return OBC_ERR_CODE_INVALID_AX25_PACKET;
  }
  memcpy(
      uplinkData,
      unstuffedPacket->data + AX25_PID_BYTES + AX25_MOD128_CONTROL_BYTES + AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES,
      AX25_INFO_BYTES);
  pktReceiveNum++;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief calculates the FCS for an ax.25 packet
 *
 * @param data uint8_t array that holds the ax25 packet data
 * @param calculatedFcs pointer to a un16_t to hold the calculated FCS
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t fcsCalculate(const uint8_t *data, uint16_t *calculatedFcs) {
  /* TODO: look into this more and make sure this is the right implementation */
  *calculatedFcs = 0xFFFF;  // Initial calculatedFcs value

  for (uint16_t i = 0; i < (AX25_MINIMUM_I_FRAME_LEN - AX25_FCS_BYTES - AX25_END_FLAG_BYTES); ++i) {
    *calculatedFcs ^= (uint16_t)data[i] << 8;

    for (uint8_t j = 0; j < 8; ++j) {
      if (*calculatedFcs & 0x8000) {
        *calculatedFcs = (*calculatedFcs << 1) ^ 0x8408;  // Polynomial X^16 + X^12 + X^5 + 1
      } else {
        *calculatedFcs <<= 1;
      }
    }
  }

  *calculatedFcs ^= 0xFFFF;  // XOR with 0xFFFF at the end
  // reverse order so that FCS can be transmitted with most significant bit first as per AX25 standard
  uint16_t reverse_num = 0;
  for (uint8_t i = 0; i < sizeof(*calculatedFcs) * 8; i++) {
    if ((*calculatedFcs & (1 << i))) reverse_num |= 1 << ((sizeof(*calculatedFcs) * 8 - 1) - i);
  }
  *calculatedFcs = reverse_num;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief checks if a received fcs is correct
 *
 * @param data the received ax.25 packet data
 * @param fcs the FCS of the received packet to be checked if it is valid or not
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was a valid fcs and error code if not
 */
static obc_error_code_t fcsCheck(const uint8_t *data, uint16_t fcs) {
  // reverse bit order of fcs to account for the fact that it was transmitted in the reverse order as the other bytes
  uint16_t reverse_num = 0;
  for (uint8_t i = 0; i < sizeof(fcs) * 8; i++) {
    if ((fcs & (1 << i))) reverse_num |= 1 << ((sizeof(fcs) * 8 - 1) - i);
  }
  fcs = reverse_num;
  uint16_t calculatedFcs = 0xFFFF;  // Initial calculatedFcs value

  for (uint16_t i = 0; i < (AX25_MINIMUM_I_FRAME_LEN - AX25_FCS_BYTES - AX25_END_FLAG_BYTES); ++i) {
    calculatedFcs ^= (uint16_t)data[i] << 8;

    for (uint8_t j = 0; j < 8; ++j) {
      if (calculatedFcs & 0x8000) {
        calculatedFcs = (calculatedFcs << 1) ^ 0x8408;  // Polynomial X^16 + X^12 + X^5 + 1
      } else {
        calculatedFcs <<= 1;
      }
    }
  }

  calculatedFcs ^= 0xFFFF;  // XOR with 0xFFFF at the end
  if (fcs != calculatedFcs) {
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param rawData unstuffed data buffer
 * @param rawDataLen length of the rawData buffer
 * @param stuffedData buffer to store the stuffed data
 * @param stuffedDataLen number of bytes filled into the stuffedData buffer
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_error_code_t bitStuffing(uint8_t *rawData, uint16_t rawDataLen, uint8_t *stuffedData,
                                    uint16_t *stuffedDataLen) {
  size_t rawOffset = 0, stuffedOffset = 8, oneCount = 0;
  uint8_t currentBit;

  // Cycle through raw data to find 1s
  for (rawOffset = 8; rawOffset < (rawDataLen - 1) * 8; ++rawOffset) {
    currentBit = (rawData[rawOffset / 8] >> (7 - (rawOffset % 8))) & 1;
    stuffedData[stuffedOffset / 8] |= (currentBit << (7 - (stuffedOffset % 8)));
    stuffedOffset++;

    if (currentBit == 1) {
      oneCount++;
      if (oneCount == 5) {
        oneCount = 0;
        stuffedData[stuffedOffset / 8] |= 0;
        stuffedOffset++;
      }
    } else {
      oneCount = 0;
    }
  }
  stuffedDataLen = ((stuffedOffset + 7) / 8) + 1;
  return OBC_ERR_CODE_SUCCESS;
}
