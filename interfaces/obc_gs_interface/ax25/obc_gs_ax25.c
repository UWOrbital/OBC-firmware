#include "obc_gs_ax25.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define AX25_U_FRAME_SABME_CMD_CONTROL 0b01101111
#define AX25_U_FRAME_DISC_CMD_CONTROL 0b01000011
#define AX25_U_FRAME_ACK_CMD_CONTROL 0b01100011
#define POLL_FINAL_BIT_OFFSET 4
#define POLL_FINAL_BIT_MASK (0x01 << POLL_FINAL_BIT_OFFSET)

#define SRC_CALLSIGN "\0\0\0\0\0\0\0"

static uint8_t pktSentNum = 0;
static uint8_t pktReceiveNum = 0;

/* Note these will need to be adjusted according to chapter 3.12 of the AX.25 Standard */
ax25_addr_t cubesatCallsign = {.data = {0}, .length = AX25_DEST_ADDR_BYTES};        // mock cubesat address
ax25_addr_t groundStationCallsign = {.data = {0}, .length = AX25_DEST_ADDR_BYTES};  // Mock Ground station address

/**
 * @brief checks for a valid s frame and performs the necessary command responses
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_gs_error_code_t sFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket);

/**
 * @brief checks for a valid i frame
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_gs_error_code_t iFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket);

/**
 * @brief recieves a U frame and performs the necessary next action
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_gs_error_code_t uFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket);

/**
 * @brief calculates the FCS for an ax.25 packet
 *
 * @param data uint8_t array that holds the ax25 packet data
 * @param dataLen total length of the data array
 * @param calculatedFcs pointer to a un16_t to hold the calculated FCS
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_gs_error_code_t fcsCalculate(const uint8_t *data, uint16_t dataLen, uint16_t *calculatedFcs);

/**
 * @brief checks if a received fcs is correct
 *
 * @param data the received ax.25 packet data
 * @param dataLen total length of the data array
 * @param fcs the FCS of the received packet to be checked if it is valid or not
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was a valid fcs and error code if not
 */
static obc_gs_error_code_t fcsCheck(const uint8_t *data, uint16_t dataLen, uint16_t fcs);

/**
 * @brief performs bit unstuffing on a receive ax.25 packet
 *
 * @param rawData unstuffed data buffer
 * @param rawDataLen length of the rawData buffer
 * @param stuffedData buffer to store the stuffed data
 * @param stuffedDataLen number of bytes filled into the stuffedData buffer
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and error code if not
 */
static obc_gs_error_code_t bitStuffing(uint8_t *rawData, uint16_t rawDataLen, uint8_t *stuffedData,
                                       uint16_t *stuffedDataLen);

obc_gs_error_code_t ax25SendIFrame(uint8_t *telemData, uint8_t telemDataLen, packed_ax25_i_frame_t *ax25Data,
                                   const ax25_addr_t *destAddress) {
  obc_gs_error_code_t errCode;

  if (telemData == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (ax25Data == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (destAddress == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  if (destAddress->length < AX25_DEST_ADDR_BYTES) {
    /* TODO: technically not an error, should be filled with spaces */
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  memset(ax25Data->data, 0, AX25_MAXIMUM_PKT_LEN);

  uint8_t ax25PacketUnstuffed[AX25_MINIMUM_I_FRAME_LEN] = {0};

  ax25PacketUnstuffed[0] = AX25_FLAG;
  // ax25PacketUnstuffed[AX25_MINIMUM_I_FRAME_LEN - 1] = AX25_FLAG;
  memcpy(ax25PacketUnstuffed + AX25_DEST_ADDR_POSITION, destAddress->data, AX25_DEST_ADDR_BYTES);
  uint8_t srcAddress[AX25_SRC_ADDR_BYTES] = SRC_CALLSIGN;
  memcpy(ax25PacketUnstuffed + AX25_SRC_ADDR_POSITION, srcAddress, AX25_SRC_ADDR_BYTES);
  ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] = (pktReceiveNum << 1);
  ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION + 1] = (pktSentNum << 1);
  ax25PacketUnstuffed[AX25_MOD128_PID_POSITION] = AX25_PID;
  memcpy(ax25PacketUnstuffed + AX25_INFO_FIELD_POSITION, telemData, telemDataLen);
  uint16_t fcs;

  errCode = fcsCalculate(ax25PacketUnstuffed, AX25_MINIMUM_I_FRAME_LEN, &fcs);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  ax25PacketUnstuffed[AX25_I_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  ax25PacketUnstuffed[AX25_I_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);
  memset(ax25Data->data, 0, sizeof(ax25Data->data));
  errCode = bitStuffing(ax25PacketUnstuffed, AX25_MINIMUM_I_FRAME_LEN, ax25Data->data, &ax25Data->length);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  ax25Data->data[ax25Data->length - 1] = AX25_FLAG;
  ax25Data->data[0] = AX25_FLAG;
  pktSentNum++;

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25SendUFrame(packed_ax25_u_frame_t *ax25Data, uint8_t cmd, uint8_t pollFinalBit,
                                   const ax25_addr_t *destAddress) {
  if (cmd > MAX_U_FRAME_CMD_VALUE) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (ax25Data == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (destAddress == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (pollFinalBit > 1) {  // poll bit should be either 1 or 0
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  obc_gs_error_code_t errCode;

  memset(ax25Data->data, 0, AX25_MAXIMUM_U_FRAME_CMD_LENGTH);

  uint8_t ax25PacketUnstuffed[AX25_MINIMUM_U_FRAME_CMD_LENGTH] = {0};

  ax25PacketUnstuffed[0] = AX25_FLAG;

  memcpy(ax25PacketUnstuffed + AX25_DEST_ADDR_POSITION, destAddress->data, AX25_DEST_ADDR_BYTES);

  uint8_t srcAddress[AX25_SRC_ADDR_BYTES] = SRC_CALLSIGN;
  memcpy(ax25PacketUnstuffed + AX25_SRC_ADDR_POSITION, srcAddress, AX25_SRC_ADDR_BYTES);

  ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] = pollFinalBit << POLL_FINAL_BIT_OFFSET;

  if (cmd == U_FRAME_CMD_ACK) {
    ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] |= AX25_U_FRAME_ACK_CMD_CONTROL;
  } else if (cmd == U_FRAME_CMD_DISC) {
    ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] |= AX25_U_FRAME_DISC_CMD_CONTROL;
  } else {
    ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] |= AX25_U_FRAME_SABME_CMD_CONTROL;
  }

  ax25PacketUnstuffed[AX25_MOD8_PID_POSITION] = AX25_PID;

  uint16_t fcs;
  errCode = fcsCalculate(ax25PacketUnstuffed, AX25_MINIMUM_U_FRAME_CMD_LENGTH, &fcs);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  ax25PacketUnstuffed[AX25_U_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  ax25PacketUnstuffed[AX25_U_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);

  errCode =
      bitStuffing(ax25PacketUnstuffed, AX25_MINIMUM_U_FRAME_CMD_LENGTH, ax25Data->data, (uint16_t *)&ax25Data->length);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  ax25Data->data[ax25Data->length - 1] = AX25_FLAG;
  ax25Data->data[0] = AX25_FLAG;

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25Recv(unstuffed_ax25_i_frame_t *unstuffedPacket) {
  if (unstuffedPacket == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  if (unstuffedPacket->length > AX25_MINIMUM_I_FRAME_LEN || unstuffedPacket->length < AX25_MINIMUM_U_FRAME_CMD_LENGTH) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  uint8_t recvAddress[AX25_SRC_ADDR_BYTES] = SRC_CALLSIGN;
  if (memcmp(unstuffedPacket->data + AX25_DEST_ADDR_POSITION, recvAddress, AX25_DEST_ADDR_BYTES) != 0) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  obc_gs_error_code_t errCode;

  // Check FCS
  uint16_t fcs = unstuffedPacket->data[unstuffedPacket->length - AX25_END_FLAG_BYTES - AX25_FCS_BYTES] << 8;
  fcs |= unstuffedPacket->data[unstuffedPacket->length - AX25_END_FLAG_BYTES - AX25_FCS_BYTES + 1];
  errCode = fcsCheck(unstuffedPacket->data, unstuffedPacket->length, fcs);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  if (!(unstuffedPacket->data[AX25_CONTROL_BYTES_POSITION] & 0x01)) {
    // check if the LSB of the control field is 0, which means it is a I frame
    errCode = iFrameRecv(unstuffedPacket);
  } else if (unstuffedPacket->data[AX25_CONTROL_BYTES_POSITION] & (0x01 << 1)) {
    // If the LSB was 1, check if the next bit is a 1 to see if it is a U Frame
    errCode = uFrameRecv(unstuffedPacket);
  } else {
    // Must be an S Frame if we reach this point
    errCode = sFrameRecv(unstuffedPacket);
  }

  return errCode;
}

obc_gs_error_code_t ax25Unstuff(uint8_t *packet, uint16_t packetLen, uint8_t *unstuffedPacket,
                                uint16_t *unstuffedPacketLen) {
  uint8_t bitCount = 0;
  uint8_t stuffingFlag = 0;
  uint16_t unstuffedBitLength = 0;  // count as bits

  // Set the first flag
  unstuffedPacket[0] = AX25_FLAG;
  unstuffedBitLength += 8;

  // loop from second byte to second last byte since first and last are the flags
  for (uint16_t stuffedPacketIndex = 1; stuffedPacketIndex < packetLen - 1; ++stuffedPacketIndex) {
    uint8_t current_byte = packet[stuffedPacketIndex];

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
      unstuffedPacket[unstuffedBitLength / 8] |= bit << (7 - (unstuffedBitLength % 8));
      unstuffedBitLength++;
    }
  }
  // Add last flag to end of the packet, rounding up
  unstuffedPacket[(unstuffedBitLength + 7) / 8] = AX25_FLAG;
  unstuffedBitLength += 8;

  // convert bits to bytes
  *unstuffedPacketLen = unstuffedBitLength / 8;

  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t sFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket) {
  if (unstuffedPacket->data[AX25_MOD128_PID_POSITION] != AX25_PID) {
    return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
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
    return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  }

  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t iFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket) {
  // first control byte will be the the after the flag and the address bytes
  // next control byte will be immediately after the previous one
  // See AX.25 standard
  if (unstuffedPacket->data[AX25_MOD128_PID_POSITION] != AX25_PID) {
    return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  }
  uint8_t controlBytes[AX25_MOD128_CONTROL_BYTES] = {
      unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES],
      unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES + 1]};

  if ((controlBytes[0] >> 1) != pktReceiveNum) {
    // TODO: implement retransmission requests
  }

  if (controlBytes[1] & 0x01) {
    // TODO: implement retransmissions
  }

  if ((controlBytes[1] >> 1) != pktSentNum) {
    // TODO: implement retransmissions
  }

  pktReceiveNum++;
  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t uFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket) {
  if (unstuffedPacket->data[AX25_MOD8_PID_POSITION] != AX25_PID) {
    return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  }

  uint8_t controlByte = unstuffedPacket->data[AX25_CONTROL_BYTES_POSITION];
  uint8_t pollFinalBit = controlByte & POLL_FINAL_BIT_MASK;

  // clear the poll/final bit from controlByte
  controlByte &= ~POLL_FINAL_BIT_MASK;

  ax25_addr_t destAddress = {0};

  // the destination address for the packet we send will be the src address of the packet we just received
  memcpy(destAddress.data, unstuffedPacket->data + AX25_SRC_ADDR_POSITION, AX25_DEST_ADDR_BYTES);
  if (controlByte == U_FRAME_CMD_CONN) {
    // Send an unnumbered acknowledgement
    packed_ax25_u_frame_t ax25Data = {0};
    // Reset the various numbering variables for the new link
    pktSentNum = 0;
    pktReceiveNum = 0;
    ax25SendUFrame(&ax25Data, U_FRAME_CMD_ACK, pollFinalBit, &destAddress);
    // Future PR will handle sending this to the proper queue
  } else if (controlByte == U_FRAME_CMD_DISC) {
    // Send an unnumbered acknowledgement
    packed_ax25_u_frame_t ax25Data = {0};
    ax25SendUFrame(&ax25Data, U_FRAME_CMD_ACK, pollFinalBit, &destAddress);
    // Future PR will handle sending this to the proper queue
  } else if (controlByte == U_FRAME_CMD_ACK) {
    // Reset the various numbering variables for the new link
    pktSentNum = 0;
    pktReceiveNum = 0;
    // acknowledgeFlag == true
    // Future PR will handle what to do here most likely some kind of state variable change
  }

  // Add more command actions as our architecture changes to need different commands
  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t fcsCalculate(const uint8_t *data, uint16_t dataLen, uint16_t *calculatedFcs) {
  /* TODO: look into this more and make sure this is the right implementation */
  *calculatedFcs = 0xFFFF;  // Initial calculatedFcs value

  for (uint16_t i = 0; i < (dataLen - AX25_FCS_BYTES - AX25_END_FLAG_BYTES); ++i) {
    *calculatedFcs ^= (uint16_t)data[i] << 8;

    for (uint8_t j = 0; j < 8; ++j) {
      if (*calculatedFcs & 0x8000) {
        *calculatedFcs = (*calculatedFcs << 1) ^ 0x8408;  // Polynomial X^16 + X^12 + X^5 + 1
      } else {
        *calculatedFcs <<= 1;
      }
    }
  }

  *calculatedFcs ^= 0xFFFF;

  // reverse order so that FCS can be transmitted with most significant bit first as per AX25 standard
  uint16_t reverse_num = 0;
  for (uint8_t i = 0; i < sizeof(*calculatedFcs) * 8; i++) {
    if ((*calculatedFcs & (1 << i))) reverse_num |= 1 << ((sizeof(*calculatedFcs) * 8 - 1) - i);
  }

  *calculatedFcs = reverse_num;

  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t fcsCheck(const uint8_t *data, uint16_t dataLen, uint16_t fcs) {
  // reverse bit order of fcs to account for the fact that it was transmitted in the reverse order as the other bytes
  uint16_t reverse_num = 0;
  for (uint8_t i = 0; i < sizeof(fcs) * 8; i++) {
    if ((fcs & (1 << i))) reverse_num |= 1 << ((sizeof(fcs) * 8 - 1) - i);
  }
  fcs = reverse_num;
  uint16_t calculatedFcs = 0xFFFF;  // Initial calculatedFcs value

  for (uint16_t i = 0; i < (dataLen - AX25_FCS_BYTES - AX25_END_FLAG_BYTES); ++i) {
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
    return OBC_GS_ERR_CODE_CORRUPTED_AX25_MSG;
  }

  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t bitStuffing(uint8_t *rawData, uint16_t rawDataLen, uint8_t *stuffedData,
                                       uint16_t *stuffedDataLen) {
  uint16_t rawOffset = 0, stuffedOffset = 8, oneCount = 0;
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

  *stuffedDataLen = ((stuffedOffset + 7) / 8) + 1;
  return OBC_GS_ERR_CODE_SUCCESS;
}
