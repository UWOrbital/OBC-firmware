#include "obc_gs_ax25.h"
#include "obc_gs_crc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define AX25_U_FRAME_SABME_CMD_CONTROL 0b01101111
// NOTE: This is defined and used since we are using mod 8 control fields (8 bit
// fields)
#define AX25_U_FRAME_SABM_CMD_CONTROL 0b00101111
#define AX25_U_FRAME_DISC_CMD_CONTROL 0b01000011
#define AX25_U_FRAME_ACK_CMD_CONTROL 0b01100011
#define POLL_FINAL_BIT_OFFSET 4
#define POLL_FINAL_BIT_MASK (0x01 << POLL_FINAL_BIT_OFFSET)

#define AX25_ADDRESS_SPACE_BYTE 0x40
#define AX25_ADDRESS_RESERVE_BIT_MASK 0b01100000
#define AX25_ADDRESS_END_FLAG 0x01

static uint8_t pktSentNum = 0;
static uint8_t pktReceiveNum = 0;

static ax25_addr_t currentLinkDestAddr;

/**
 * @brief reverses the given number and returns the reversed number
 *
 * @param numToReverse: the number that should be reversed
 *
 * @return uint16_t - the reversed number
 */
static inline uint16_t reverseUint16(uint16_t numToReverse);

/**
 * @brief checks for a valid s frame and performs the necessary command
 * responses
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and
 * error code if not
 */
static obc_gs_error_code_t sFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket);

/**
 * @brief checks for a valid i frame
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and
 * error code if not
 */
static obc_gs_error_code_t iFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket);

/**
 * @brief recieves a U frame and performs the necessary next action
 *
 * @param unstuffedPacket unstuffed ax.25 packet
 * @param uFrameCmd buffer to store the received command if the frame was a U
 * frame
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was successful and
 * error code if not
 */
static obc_gs_error_code_t uFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket, u_frame_cmd_t *command);

/**
 * @brief calculates the FCS for an ax.25 packet
 *
 * @param data uint8_t array that holds the ax25 packet data
 * @param dataLen total length of the data array
 * @param calculatedFcs pointer to a un16_t to hold the calculated FCS
 */
static void fcsCalculate(const uint8_t *data, uint16_t dataLen, uint16_t *calculatedFcs);

/**
 * @brief checks if a received fcs is correct
 *
 * @param data the received ax.25 packet data
 * @param dataLen total length of the data array
 * @param fcs the FCS of the received packet to be checked if it is valid or not
 *
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_SUCCESS if it was a valid fcs and
 * error code if not
 */
static obc_gs_error_code_t fcsCheck(const uint8_t *data, uint16_t dataLen, uint16_t fcs);

obc_gs_error_code_t ax25SendIFrameWithFlagSharing(uint8_t *telemData, uint32_t telemDataLen, uint8_t *ax25Data,
                                                  uint32_t ax25DataLen, const ax25_addr_t *destAddress) {
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
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  uint8_t numOfFrames = (telemDataLen + AX25_INFO_BYTES - 1) / AX25_INFO_BYTES;  // Number of frames and rounding up
  if (ax25DataLen < ((uint32_t)(numOfFrames * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + 1)) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  memset(ax25Data, 0, ax25DataLen);

  uint8_t remainingDataBytes = telemDataLen;
  uint8_t frameStart = 0;

  for (frameStart = 0; frameStart < numOfFrames; ++frameStart) {
    ax25Data[frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG] = AX25_FLAG;

    memcpy(ax25Data + (frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_DEST_ADDR_POSITION, destAddress->data,
           AX25_DEST_ADDR_BYTES);
    ax25_addr_t srcAddr = {0};
    ax25GetSourceAddress(&srcAddr, GROUND_STATION_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
    memcpy(ax25Data + (frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_SRC_ADDR_POSITION, srcAddr.data,
           AX25_SRC_ADDR_BYTES);

    ax25Data[(frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_CONTROL_BYTES_POSITION] = (pktReceiveNum << 1);
    ax25Data[(frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_CONTROL_BYTES_POSITION + 1] = (pktSentNum << 1);
    ax25Data[(frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_MOD128_PID_POSITION] = AX25_PID;
    if (remainingDataBytes >= AX25_INFO_BYTES) {
      memcpy(ax25Data + (frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_INFO_FIELD_POSITION,
             telemData + (frameStart * AX25_INFO_BYTES), AX25_INFO_BYTES);
    } else {
      memcpy(ax25Data + (frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_INFO_FIELD_POSITION,
             telemData + (frameStart * AX25_INFO_BYTES), remainingDataBytes);
    }

    uint16_t fcs;
    fcsCalculate(ax25Data + (frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG), AX25_MINIMUM_I_FRAME_LEN, &fcs);

    ax25Data[(frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_I_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
    ax25Data[(frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + AX25_I_FRAME_FCS_POSITION + 1] =
        (uint8_t)(fcs & 0xFF);
    pktSentNum++;
    remainingDataBytes -= AX25_INFO_BYTES;
  }
  ax25Data[frameStart * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG] = AX25_FLAG;
  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25SendIFrame(uint8_t *telemData, uint8_t telemDataLen, unstuffed_ax25_i_frame_t *ax25Data) {
  if (telemData == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (ax25Data == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (currentLinkDestAddr.length != AX25_DEST_ADDR_BYTES) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  if (currentLinkDestAddr.length < AX25_DEST_ADDR_BYTES) {
    /* TODO: technically not an error, should be filled with spaces */
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  // The following checks stop the data from corrupting in the mod8
  // implementation
  if (pktSentNum >= MAX_CONTINUOUS_PACKETS_ALLOWED) {
    pktSentNum = 0;
  }
  if (pktReceiveNum >= MAX_CONTINUOUS_PACKETS_ALLOWED) {
    pktReceiveNum = 0;
  }

  memset(ax25Data->data, 0, AX25_MINIMUM_I_FRAME_LEN);
  ax25Data->length = AX25_MINIMUM_I_FRAME_LEN;

  ax25Data->data[0] = AX25_FLAG;
  // ax25Data->data[AX25_MINIMUM_I_FRAME_LEN - 1] = AX25_FLAG;
  ax25_addr_t srcAddr = {0};
  ax25GetSourceAddress(&srcAddr, GROUND_STATION_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
  memcpy(ax25Data->data + AX25_DEST_ADDR_POSITION, currentLinkDestAddr.data, AX25_DEST_ADDR_BYTES);
  memcpy(ax25Data->data + AX25_SRC_ADDR_POSITION, srcAddr.data, AX25_SRC_ADDR_BYTES);

  // NOTE: We assume that the P bit is 0
  ax25Data->data[AX25_CONTROL_BYTES_POSITION] = (pktReceiveNum << 5) | (pktSentNum << 1);
  ax25Data->data[AX25_MOD8_PID_POSITION] = AX25_PID;

  // NOTE: This is for the MOD128 Implementation
  // ax25Data->data[AX25_CONTROL_BYTES_POSITION + 1] = (pktSentNum << 1);
  // ax25Data->data[AX25_MOD128_PID_POSITION] = AX25_PID;
  memcpy(ax25Data->data + AX25_INFO_FIELD_POSITION, telemData, telemDataLen);

  uint16_t fcs;
  fcsCalculate(ax25Data->data + AX25_START_FLAG_BYTES, AX25_MINIMUM_I_FRAME_LEN, &fcs);

  ax25Data->data[AX25_I_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  ax25Data->data[AX25_I_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);
  ax25Data->data[ax25Data->length - 1] = AX25_FLAG;
  ax25Data->data[0] = AX25_FLAG;

  pktSentNum++;
  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25SendUFrame(packed_ax25_u_frame_t *ax25Data, uint8_t cmd, uint8_t pollFinalBit) {
  if (cmd > MAX_U_FRAME_CMD_VALUE) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (ax25Data == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (currentLinkDestAddr.length != AX25_DEST_ADDR_BYTES) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (pollFinalBit > 1) {  // poll bit should be either 1 or 0
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  obc_gs_error_code_t errCode;

  memset(ax25Data->data, 0, AX25_MAXIMUM_U_FRAME_CMD_LENGTH);

  uint8_t ax25PacketUnstuffed[AX25_MINIMUM_U_FRAME_CMD_LENGTH] = {0};

  ax25PacketUnstuffed[0] = AX25_FLAG;

  ax25_addr_t srcAddr = {0};
  ax25GetSourceAddress(&srcAddr, GROUND_STATION_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
  memcpy(ax25PacketUnstuffed + AX25_DEST_ADDR_POSITION, currentLinkDestAddr.data, AX25_DEST_ADDR_BYTES);

  memcpy(ax25PacketUnstuffed + AX25_SRC_ADDR_POSITION, srcAddr.data, AX25_SRC_ADDR_BYTES);

  ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] = pollFinalBit << POLL_FINAL_BIT_OFFSET;

  if (cmd == U_FRAME_CMD_ACK) {
    ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] |= AX25_U_FRAME_ACK_CMD_CONTROL;
  } else if (cmd == U_FRAME_CMD_DISC) {
    ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] |= AX25_U_FRAME_DISC_CMD_CONTROL;
  } else {
    ax25PacketUnstuffed[AX25_CONTROL_BYTES_POSITION] |= AX25_U_FRAME_SABM_CMD_CONTROL;
  }

  uint16_t fcs;
  fcsCalculate(ax25PacketUnstuffed + 1, AX25_MINIMUM_U_FRAME_CMD_LENGTH, &fcs);

  ax25PacketUnstuffed[AX25_U_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  ax25PacketUnstuffed[AX25_U_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);
  ax25PacketUnstuffed[AX25_MINIMUM_U_FRAME_CMD_LENGTH - 1] = AX25_FLAG;

  errCode =
      ax25Stuff(ax25PacketUnstuffed, AX25_MINIMUM_U_FRAME_CMD_LENGTH, ax25Data->data, (uint16_t *)&ax25Data->length);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25Recv(unstuffed_ax25_i_frame_t *unstuffedPacket, u_frame_cmd_t *command) {
  if (unstuffedPacket == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (unstuffedPacket->length > AX25_MINIMUM_I_FRAME_LEN || unstuffedPacket->length < AX25_MINIMUM_U_FRAME_CMD_LENGTH) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  ax25_addr_t destAddr = {0};
  ax25GetDestAddress(&destAddr, CUBE_SAT_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
  ax25_addr_t destAddrAlternate = {0};
  ax25GetDestAddress(&destAddrAlternate, GROUND_STATION_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
  if (memcmp(unstuffedPacket->data + AX25_DEST_ADDR_POSITION, destAddr.data, AX25_DEST_ADDR_BYTES) != 0 &&
      memcmp(unstuffedPacket->data + AX25_DEST_ADDR_POSITION, destAddrAlternate.data, AX25_DEST_ADDR_BYTES) != 0) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  obc_gs_error_code_t errCode;

  // Check FCS
  uint16_t fcs = unstuffedPacket->data[unstuffedPacket->length - AX25_END_FLAG_BYTES - AX25_FCS_BYTES] << 8;
  fcs |= unstuffedPacket->data[unstuffedPacket->length - AX25_END_FLAG_BYTES - AX25_FCS_BYTES + 1];
  errCode = fcsCheck(unstuffedPacket->data + 1, unstuffedPacket->length, fcs);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return errCode;
  }

  if (!(unstuffedPacket->data[AX25_CONTROL_BYTES_POSITION] & 0x01)) {
    // check if the LSB of the control field is 0, which means it is a I frame
    errCode = iFrameRecv(unstuffedPacket);
  } else if (unstuffedPacket->data[AX25_CONTROL_BYTES_POSITION] & (0x01 << 1)) {
    // If the LSB was 1, check if the next bit is a 1 to see if it is a U Frame
    errCode = uFrameRecv(unstuffedPacket, command);
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

  // loop from second byte to second last byte since first and last are the
  // flags
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

  // Let these bits at the end that are 0 and are a result of the unstuffing
  // process
  uint8_t tailBytes = 0;

  while (unstuffedPacket[(unstuffedBitLength / 8) - tailBytes] == 0x00 && tailBytes < AX25_INFO_BYTES) {
    tailBytes++;
  }
  // Add last flag to end of the packet, rounding up and removing any 0
  // bytes at the end as a result of unstuffing
  if (tailBytes == 0) {
    unstuffedPacket[(unstuffedBitLength + 7) / 8] = AX25_FLAG;
    *unstuffedPacketLen = ((unstuffedBitLength + 7) / 8) + 1;
  } else {
    unstuffedPacket[(unstuffedBitLength / 8) - tailBytes + 1] = AX25_FLAG;
    *unstuffedPacketLen = unstuffedBitLength / 8 - tailBytes + 2;
  }

  // convert bits to bytes

  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t sFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket) {
  ax25_addr_t srcAddr = {0};
  ax25GetSourceAddress(&srcAddr, GROUND_STATION_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
  if (memcmp(unstuffedPacket->data + AX25_SRC_ADDR_POSITION, srcAddr.data, AX25_SRC_ADDR_BYTES) != 0) {
    return OBC_GS_ERR_CODE_INVALID_TNC;
  }
  if (unstuffedPacket->data[AX25_MOD8_PID_POSITION] != AX25_PID) {
    return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  }

  // NOTE: This if for the mod128 implementation
  // if (unstuffedPacket->data[AX25_MOD128_PID_POSITION] != AX25_PID) {
  //   return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  // }

  // NOTE: this is for a mod128 implementation
  // if (unstuffedPacket->data[AX25_MOD128_PID_POSITION] != AX25_PID) {
  //   return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  // }

  uint8_t controlBytes[AX25_MOD8_CONTROL_BYTES] = {unstuffedPacket->data[AX25_ADDRESS_BYTES + 1]};

  // NOTE: This is for the mod128 implementation
  // uint8_t controlBytes[AX25_MOD128_CONTROL_BYTES] = {
  //     unstuffedPacket->data[AX25_ADDRESS_BYTES + 1],
  //     unstuffedPacket->data[AX25_ADDRESS_BYTES + 2]};

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
  ax25_addr_t srcAddr = {0};
  ax25GetSourceAddress(&srcAddr, GROUND_STATION_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);
  // first control byte will be the the after the flag and the address bytes
  // next control byte will be immediately after the previous one
  // See AX.25 standard
  if (memcmp(unstuffedPacket->data + AX25_SRC_ADDR_POSITION, srcAddr.data, AX25_SRC_ADDR_BYTES) != 0) {
    return OBC_GS_ERR_CODE_INVALID_TNC;
  }

  if (unstuffedPacket->data[AX25_MOD8_PID_POSITION] != AX25_PID && unstuffedPacket->data[AX25_MOD8_PID_POSITION] != 0) {
    return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  }

  // NOTE: This check if for mod 128 implementation
  // if (unstuffedPacket->data[AX25_MOD128_PID_POSITION] != AX25_PID) {
  //   return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
  // }

  uint8_t controlBytes[AX25_MOD8_CONTROL_BYTES] = {unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES]};

  // NOTE: This is the mod 128 implementation
  // uint8_t controlBytes[AX25_MOD128_CONTROL_BYTES] = {
  //     unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES],
  //     unstuffedPacket->data[AX25_ADDRESS_BYTES + AX25_START_FLAG_BYTES + 1]};

  if ((controlBytes[0] >> 1) != pktReceiveNum) {
    // TODO: implement retransmission requests
  }

  // NOTE: This is for the mod 128 implementation
  // if (controlBytes[1] & 0x01) {
  //   // TODO: implement retransmissions
  // }
  // if ((controlBytes[1] >> 1) != pktSentNum) {
  //   // TODO: implement retransmissions
  // }

  pktReceiveNum++;
  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t uFrameRecv(unstuffed_ax25_i_frame_t *unstuffedPacket, u_frame_cmd_t *command) {
  uint8_t controlByte = unstuffedPacket->data[AX25_CONTROL_BYTES_POSITION];
  // uint8_t pollFinalBit = controlByte & POLL_FINAL_BIT_MASK; TODO: figure out
  // how to handle poll/control bits

  // clear the poll/final bit from controlByte
  controlByte &= ~POLL_FINAL_BIT_MASK;

  ax25_addr_t destAddr = {0};
  ax25GetDestAddress(&destAddr, CUBE_SAT_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID, DEFAULT_CONTROL_BIT);

  // the destination address for the packet we send will be the src address of
  // the packet we just received
  memcpy(destAddr.data, unstuffedPacket->data + AX25_SRC_ADDR_POSITION, AX25_DEST_ADDR_BYTES);
  if (controlByte == AX25_U_FRAME_SABM_CMD_CONTROL) {
    // Reset the various numbering variables for the new link
    pktSentNum = 0;
    pktReceiveNum = 0;
    if (currentLinkDestAddr.length != AX25_DEST_ADDR_BYTES) {
      setCurrentLinkDestCallSign(CUBE_SAT_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID);
    }
    *command = U_FRAME_CMD_CONN;
    return OBC_GS_ERR_CODE_SUCCESS;
  } else if (controlByte == AX25_U_FRAME_DISC_CMD_CONTROL) {
    if (memcmp(unstuffedPacket->data + AX25_SRC_ADDR_POSITION, currentLinkDestAddr.data, AX25_SRC_ADDR_BYTES - 1) !=
        0) {
      return OBC_GS_ERR_CODE_INVALID_TNC;
    }
    *command = U_FRAME_CMD_DISC;
    return OBC_GS_ERR_CODE_SUCCESS;
  } else if (controlByte == AX25_U_FRAME_ACK_CMD_CONTROL) {
    if (memcmp(unstuffedPacket->data + AX25_SRC_ADDR_POSITION, currentLinkDestAddr.data, AX25_SRC_ADDR_BYTES - 1) !=
        0) {
      return OBC_GS_ERR_CODE_INVALID_TNC;
    }
    // Reset the various numbering variables for the new link
    pktSentNum = 0;
    pktReceiveNum = 0;
    *command = U_FRAME_CMD_ACK;
    return OBC_GS_ERR_CODE_SUCCESS;
  }

  // Add more command actions as our architecture changes to need different
  // commands
  return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
}

static void fcsCalculate(const uint8_t *data, uint16_t dataLen, uint16_t *calculatedFcs) {
  *calculatedFcs = calculateCrc16Ccitt(data, dataLen - AX25_FCS_BYTES - AX25_TOTAL_FLAG_BYTES);

  // reverse order so that FCS can be transmitted with most significant bit
  // first as per AX25 standard
  *calculatedFcs = reverseUint16(*calculatedFcs);
}

static obc_gs_error_code_t fcsCheck(const uint8_t *data, uint16_t dataLen, uint16_t fcs) {
  // reverse bit order of fcs to account for the fact that it was transmitted in
  // the reverse order as the other bytes
  fcs = reverseUint16(fcs);

  uint16_t calculatedFcs = calculateCrc16Ccitt(data, dataLen - AX25_FCS_BYTES - AX25_TOTAL_FLAG_BYTES);

  if (fcs != calculatedFcs) {
    return OBC_GS_ERR_CODE_CORRUPTED_AX25_MSG;
  }

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25Stuff(uint8_t *rawData, uint16_t rawDataLen, uint8_t *stuffedData, uint16_t *stuffedDataLen) {
  if (rawData == NULL || stuffedData == NULL || stuffedDataLen == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

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
  stuffedData[0] = AX25_FLAG;
  stuffedData[*stuffedDataLen - 1] = AX25_FLAG;
  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25GetDestAddress(ax25_addr_t *address, uint8_t callSign[], uint8_t callSignLength, uint8_t ssid,
                                       uint8_t controlBit) {
  if (address == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (callSign == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (ssid > 16) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (controlBit > 2) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (callSignLength > CALL_SIGN_BYTES) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  for (uint8_t i = 0; i < callSignLength; ++i) {
    address->data[i] = callSign[i] << 1;
  }

  for (uint8_t i = callSignLength; i < CALL_SIGN_BYTES; ++i) {
    address->data[i] = AX25_ADDRESS_SPACE_BYTE;
  }

  address->length = AX25_DEST_ADDR_BYTES;

  controlBit = controlBit << 7;
  ssid = ssid << 1;
  ssid |= AX25_ADDRESS_RESERVE_BIT_MASK;
  address->data[address->length - 1] = ssid | controlBit;

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t ax25GetSourceAddress(ax25_addr_t *address, uint8_t callSign[], uint8_t callSignLength, uint8_t ssid,
                                         uint8_t controlBit) {
  if (address == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (callSign == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (ssid > 16) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (controlBit > 2) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (callSignLength > CALL_SIGN_BYTES) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  for (uint8_t i = 0; i < callSignLength; ++i) {
    address->data[i] = callSign[i] << 1;
  }

  for (uint8_t i = callSignLength; i < CALL_SIGN_BYTES; ++i) {
    address->data[i] = AX25_ADDRESS_SPACE_BYTE;
  }

  address->length = AX25_SRC_ADDR_BYTES;

  controlBit = controlBit << 7;
  ssid = ssid << 1;
  ssid |= AX25_ADDRESS_RESERVE_BIT_MASK;  // set R bits to 1 unless specified
  address->data[address->length - 1] = ssid | controlBit | AX25_ADDRESS_END_FLAG;

  return OBC_GS_ERR_CODE_SUCCESS;
}

void setCurrentLinkDestCallSign(uint8_t *destCallSign, uint8_t destCallSignLength, uint8_t ssid) {
  ax25_addr_t destAddr = {0};
  ax25GetDestAddress(&destAddr, destCallSign, destCallSignLength, ssid, 0);
  memcpy(&currentLinkDestAddr, &destAddr, sizeof(ax25_addr_t));
}

void clearCurrentLinkDestAddress(void) { memset(&currentLinkDestAddr, 0, sizeof(ax25_addr_t)); }

static inline uint16_t reverseUint16(uint16_t numToReverse) {
  uint16_t reverseNum = 0;
  for (uint8_t i = 0; i < sizeof(numToReverse) * 8; i++) {
    if ((numToReverse & (1 << i))) reverseNum |= 1 << ((sizeof(numToReverse) * 8 - 1) - i);
  }
  return reverseNum;
}
