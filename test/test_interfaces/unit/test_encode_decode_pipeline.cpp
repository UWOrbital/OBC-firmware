#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"
#include "obc_gs_command_pack.h"
#include "obc_gs_command_unpack.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_id.h"

#include <math.h>
#include <string.h>

#include <gtest/gtest.h>
#include <stdio.h>
#include <cstdint>
#include <iostream>

// TEST: A simulated send with the entire pipline (some bits are also flipped before sending to test fec)
TEST(TestEncodeDecode, sendData) {
  uint8_t data[RS_DECODED_SIZE];
  uint32_t seed = 0x4A25C3;  // random number
  for (uint16_t i = 0; i < RS_DECODED_SIZE; ++i) {
    // Pseudorandom generation using a simple algorithm
    seed = (seed * 1103515245 + 12345) % (1 << 31);
    data[i] = (uint8_t)(seed & 0xFF);
  }
  // Create aes data and encrypt
  uint8_t key[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx((uint8_t *)key);
  aes_data_t aesData = {.iv = {0}, .ciphertext = data, .ciphertextLen = RS_DECODED_SIZE};
  memset(aesData.iv, 1, AES_IV_SIZE);
  uint8_t output[RS_DECODED_SIZE];
  ASSERT_EQ(aes128Decrypt(&aesData, output, RS_DECODED_SIZE), 0);

  setCurrentLinkDestCallSign((uint8_t *)"AKITO", CALLSIGN_LENGTH, DEFAULT_SSID);

  // Create fec data and encode
  packed_rs_packet_t fecData = {0};
  ASSERT_EQ(rsEncode(output, &fecData), OBC_GS_ERR_CODE_SUCCESS);

  // Create ax25 Frame
  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  ASSERT_EQ(ax25SendIFrame((uint8_t *)&fecData.data, RS_ENCODED_SIZE, &unstuffedAx25Data), OBC_GS_ERR_CODE_SUCCESS);

  // Stuff the ax25 Frame
  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, (uint8_t *)ax25Data.data, &ax25Data.length),
            OBC_GS_ERR_CODE_SUCCESS);

  // Manipulate some bits for testing purposes
  ax25Data.data[40] ^= 0b10000001;
  ax25Data.data[220] ^= 0b10100011;

  // Print out the ax25 data in hex to use in python testing
  // NOTE: Uncomment to see output when running c tests
  // std::cout << std::endl << "Frame Hex Data:" << std::endl;
  // for (int i = 0; i < ax25Data.length; i++) {
  //   printf(" 0x%x", ax25Data.data[i]);
  // }
  // std::cout << std::endl << "End of Frame Hex Data" << std::endl;
}

// TEST: A simulated receive with the entire pipline
TEST(TestEncodeDecode, receiveData) {
  // Data from the python encoding
  uint8_t python_data[280] = {
      0x7e, 0x82, 0x96, 0x92, 0xa8, 0x9e, 0x40, 0x60, 0x82, 0xa8, 0x98, 0x82, 0xa6, 0x40, 0x61, 0x0,  0x0,  0xb6, 0xb7,
      0x64, 0xeb, 0xde, 0x8f, 0x6b, 0xd5, 0xdd, 0x53, 0xc2, 0xce, 0xc4, 0x4a, 0xb,  0x78, 0xdd, 0x83, 0xe8, 0x4a, 0x53,
      0xbe, 0xc7, 0x9d, 0x4,  0x75, 0xd5, 0x99, 0xad, 0x1c, 0x8c, 0x8b, 0x23, 0x89, 0xdc, 0xb2, 0xb3, 0x8,  0x89, 0xf2,
      0x71, 0x61, 0xdc, 0x1,  0xcf, 0x1e, 0x34, 0xd5, 0xf0, 0xec, 0x9f, 0x21, 0xf7, 0x3a, 0xdf, 0x51, 0xdb, 0x76, 0x19,
      0x84, 0xc1, 0x80, 0x7c, 0xde, 0x47, 0x1c, 0xe7, 0xc5, 0x7d, 0xb8, 0xe1, 0xcf, 0x5d, 0xa5, 0x76, 0xdf, 0x73, 0x22,
      0xe7, 0xce, 0x10, 0xc8, 0x5b, 0x65, 0xb0, 0x13, 0x9c, 0x3e, 0xdf, 0x1e, 0xf6, 0x4f, 0x87, 0x9,  0xdb, 0xe9, 0xbe,
      0xe,  0x13, 0xb1, 0x47, 0x2c, 0x2a, 0x39, 0xbe, 0x20, 0x86, 0x2,  0x8,  0x35, 0xef, 0xb,  0x7b, 0x80, 0xac, 0x97,
      0x1,  0x39, 0x2,  0xbe, 0x94, 0x48, 0x2,  0x9b, 0x9b, 0x7c, 0xef, 0xb6, 0xf0, 0x6d, 0xb5, 0x7d, 0xd8, 0x8b, 0xb2,
      0x2a, 0x8a, 0x0,  0x1e, 0xeb, 0x7d, 0x5f, 0x49, 0xcf, 0x5f, 0x58, 0xe9, 0x98, 0x6b, 0x55, 0x8b, 0x3e, 0x9c, 0x54,
      0x9c, 0x5c, 0x5d, 0xea, 0x10, 0x14, 0x91, 0x7d, 0xd7, 0x11, 0x7a, 0x1e, 0xaf, 0x52, 0x17, 0x5f, 0x77, 0x50, 0xe8,
      0xe7, 0x72, 0x9d, 0xf,  0x75, 0x21, 0xe3, 0xe0, 0xc0, 0x9d, 0x92, 0x9e, 0x2c, 0x5e, 0xe,  0x92, 0x22, 0x92, 0x8d,
      0x6d, 0x70, 0xde, 0x28, 0x12, 0x2a, 0xd,  0x41, 0xf5, 0x86, 0x78, 0xc4, 0x19, 0x97, 0x75, 0x92, 0xe0, 0xe8, 0x95,
      0x9e, 0x16, 0x47, 0x19, 0x64, 0xc0, 0x29, 0xe4, 0x59, 0x52, 0xe4, 0xba, 0x54, 0x7,  0xdb, 0xdf, 0x5,  0xe4, 0x9e,
      0xd,  0x9,  0xc3, 0x79, 0xb5, 0xd7, 0x2a, 0x1f, 0x24, 0xe0, 0xea, 0x54, 0x9f, 0x31, 0x59, 0xf,  0xb8, 0x1b, 0x7c,
      0x34, 0xf0, 0x16, 0xd3, 0x64, 0xf6, 0xd8, 0x1f, 0x53, 0x72, 0xf5, 0x37, 0x70, 0x7e};

  // Create a ax25 packed frame with the length of the file
  packed_ax25_i_frame_t ax25Data = {.length = 280};
  memcpy(ax25Data.data, python_data, 280);

  // Unstuff the frame
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);

  // Apply forward error correction
  initRs();
  packed_rs_packet_t rsData;
  memcpy(rsData.data, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_ENCODED_SIZE);
  ASSERT_EQ(rsDecode(&rsData, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_DECODED_SIZE), 0);

  // Now actually breakdown the frame and see if it's valid
  u_frame_cmd_t command;
  ASSERT_EQ(ax25Recv(&unstuffedPacket, &command), OBC_GS_ERR_CODE_SUCCESS);

  // Use AES to decrypt
  uint8_t key[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx((uint8_t *)key);
  aes_data_t aesData = {
      .iv = {0}, .ciphertext = unstuffedPacket.data + AX25_INFO_FIELD_POSITION, .ciphertextLen = RS_DECODED_SIZE};
  memset(aesData.iv, 1, AES_IV_SIZE);
  uint8_t output[RS_DECODED_SIZE];
  aes128Decrypt(&aesData, output, RS_DECODED_SIZE);

  // Expected data and we check if it matches
  uint8_t expected_data[RS_DECODED_SIZE] = {
      117, 228, 228, 188, 138, 172, 20,  228, 199, 147, 61,  58,  39,  170, 53,  0,   173, 147, 136, 54,  107, 159, 64,
      212, 17,  178, 75,  39,  140, 254, 48,  230, 86,  5,   224, 131, 111, 196, 45,  192, 91,  245, 32,  201, 163, 156,
      96,  48,  88,  63,  17,  3,   241, 135, 136, 5,   86,  61,  254, 197, 63,  254, 207, 216, 204, 227, 175, 182, 26,
      27,  191, 146, 222, 77,  147, 250, 202, 6,   143, 250, 183, 244, 81,  70,  166, 41,  145, 136, 154, 218, 80,  227,
      85,  44,  236, 36,  173, 49,  197, 103, 66,  54,  151, 236, 221, 192, 21,  5,   102, 178, 72,  84,  219, 254, 184,
      139, 31,  212, 240, 128, 32,  169, 30,  142, 14,  65,  35,  47,  91,  251, 158, 98,  144, 254, 29,  167, 104, 233,
      127, 197, 110, 18,  139, 195, 128, 243, 198, 149, 219, 110, 72,  11,  6,   124, 82,  247, 207, 202, 34,  227, 245,
      179, 4,   137, 102, 211, 253, 229, 213, 233, 23,  80,  139, 147, 201, 173, 179, 167, 82,  88,  32,  166, 179, 190,
      67,  93,  216, 228, 189, 144, 60,  252, 30,  247, 70,  166, 231, 242, 183, 191, 106, 236, 5,   72,  253, 72,  117,
      122, 35,  159, 250, 160, 38,  141, 53,  156, 103, 45,  194, 0,   8,   234, 229};
  for (int i = 0; i < RS_DECODED_SIZE; i++) {
    EXPECT_EQ(output[i], expected_data[i]);
  }
}

TEST(TestEncodeDecode, uFrameReceive) {
  uint8_t pythonData[20] = {0x7e, 0x82, 0x96, 0x92, 0xa8, 0x9e, 0x40, 0x60, 0x82, 0xa8,
                            0x98, 0x82, 0xa6, 0x40, 0x61, 0x3e, 0xcd, 0x3d, 0x80, 0x7e};
  packed_ax25_i_frame_t ax25Data = {.length = 20};
  memcpy(ax25Data.data, pythonData, 20);

  // Unstuff the frame
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);

  // NOTE: Uncomment to see frame data
  // std::cout << std::endl << "Frame Hex Data:" << std::endl;
  // for (int i = 0; i < unstuffedPacket.length; i++) {
  //   printf(" 0x%x", unstuffedPacket.data[i]);
  // }
  // std::cout << std::endl << "End of Frame Hex Data" << std::endl;

  u_frame_cmd_t command;
  ASSERT_EQ(ax25Recv(&unstuffedPacket, &command), OBC_GS_ERR_CODE_SUCCESS);
}

TEST(TestEncodeDecode, uFrameSend) {
  packed_ax25_u_frame_t ax25Data = {0};
  uint8_t pollFinalBit = 1;

  setCurrentLinkDestCallSign(CUBE_SAT_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID);
  ASSERT_EQ(ax25SendUFrame(&ax25Data, U_FRAME_CMD_CONN, pollFinalBit), OBC_GS_ERR_CODE_SUCCESS);

  // NOTE: Uncomment to see frame data
  // std::cout << std::endl << "Frame Hex Data:" << std::endl;
  // for (int i = 0; i < ax25Data.length; i++) {
  //   printf(" 0x%x", ax25Data.data[i]);
  // }
  // std::cout << std::endl << "End of Frame Hex Data" << std::endl;
}

TEST(TestEncodeDecode, receiveCommandData) {
  uint8_t python_data[279] = {
      0x7e, 0x82, 0x96, 0x92, 0xa8, 0x9e, 0x40, 0x60, 0x82, 0xa8, 0x98, 0x82, 0xa6, 0x40, 0x61, 0x0,  0x0,  0xc6, 0x52,
      0x80, 0x57, 0x54, 0x21, 0x7d, 0x98, 0x8d, 0x60, 0x7d, 0xf4, 0xc6, 0xac, 0xe7, 0xc7, 0x87, 0x1,  0x7,  0x8a, 0x2c,
      0xce, 0x5,  0xea, 0x0,  0x6,  0x51, 0xd4, 0x13, 0x88, 0xc0, 0x2c, 0xad, 0x82, 0x29, 0x24, 0x9a, 0x3e, 0x30, 0x50,
      0x9e, 0x87, 0xcf, 0x3,  0x1f, 0x36, 0xdb, 0x8d, 0x3e, 0xd6, 0xf6, 0xe5, 0x3c, 0x3e, 0x98, 0x30, 0x74, 0x76, 0x98,
      0x9e, 0x52, 0xfb, 0xe8, 0x2,  0x33, 0xed, 0x0,  0x46, 0x54, 0x8f, 0x8c, 0xe4, 0x17, 0x22, 0x5c, 0xb7, 0x63, 0x1e,
      0xe9, 0xc3, 0x77, 0xb1, 0x92, 0x10, 0x52, 0x3c, 0xfb, 0xa9, 0xa9, 0xbc, 0x9d, 0x26, 0xcd, 0x9e, 0xe3, 0x55, 0x48,
      0xe2, 0x29, 0x4d, 0x2c, 0xfa, 0x2c, 0x8b, 0xd6, 0xdd, 0x8b, 0x90, 0x6b, 0xa5, 0xb5, 0x3e, 0xa6, 0x56, 0x69, 0xa2,
      0xbc, 0x6f, 0x15, 0x28, 0x52, 0xc0, 0x34, 0x75, 0xc5, 0x60, 0xf0, 0x9f, 0x17, 0x4c, 0xf9, 0x3,  0x6f, 0x23, 0x86,
      0x97, 0x27, 0x68, 0x34, 0x8d, 0xb4, 0x3,  0x28, 0x53, 0x4e, 0x84, 0x2c, 0x9,  0x5b, 0x60, 0xe9, 0x47, 0xc3, 0x4f,
      0x4e, 0xe1, 0x7d, 0xab, 0x79, 0xd8, 0xfa, 0xe2, 0xdb, 0xe9, 0x8f, 0xa3, 0xe3, 0xeb, 0x34, 0x81, 0x48, 0x6d, 0xb3,
      0xb0, 0x3c, 0xad, 0xb2, 0x4d, 0xd9, 0xc6, 0xdf, 0x50, 0x7a, 0x84, 0xd8, 0xce, 0x1f, 0x72, 0x23, 0x31, 0x56, 0xa2,
      0x7,  0xc2, 0x73, 0x91, 0xcb, 0x20, 0x70, 0xed, 0x89, 0x32, 0x98, 0xe6, 0xc8, 0x9d, 0x82, 0xc4, 0x57, 0x25, 0x1,
      0xda, 0x60, 0xe3, 0xcf, 0x88, 0xb4, 0x3a, 0x64, 0x51, 0x9c, 0xa7, 0x64, 0xa8, 0x48, 0xb8, 0x54, 0x6f, 0x4f, 0x3a,
      0xcf, 0x99, 0xd3, 0x19, 0xd7, 0x26, 0xc4, 0xb5, 0x50, 0xe7, 0x9e, 0x8d, 0x3e, 0x67, 0x7b, 0x1d, 0x62, 0xa7, 0xaf,
      0x9c, 0xed, 0x76, 0x68, 0x1a, 0xbc, 0x60, 0xa5, 0x58, 0xa5, 0x99, 0x16, 0x7e};

  // Create a ax25 packed frame with the length of the file
  packed_ax25_i_frame_t ax25Data = {.length = 279};
  memcpy(ax25Data.data, python_data, 279);

  // Unstuff the frame
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);

  // Apply forward error correction
  initRs();
  packed_rs_packet_t rsData;
  memcpy(rsData.data, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_ENCODED_SIZE);
  ASSERT_EQ(rsDecode(&rsData, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_DECODED_SIZE), 0);

  // Now actually breakdown the frame and see if it's valid
  u_frame_cmd_t command;
  ASSERT_EQ(ax25Recv(&unstuffedPacket, &command), OBC_GS_ERR_CODE_SUCCESS);

  // Use AES to decrypt
  uint8_t key[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx((uint8_t *)key);
  aes_data_t aesData = {
      .iv = {0}, .ciphertext = unstuffedPacket.data + AX25_INFO_FIELD_POSITION, .ciphertextLen = RS_DECODED_SIZE};
  memset(aesData.iv, 1, AES_IV_SIZE);
  uint8_t output[RS_DECODED_SIZE];
  aes128Decrypt(&aesData, output, RS_DECODED_SIZE);
  // NOTE: Uncomment to see frame data
  std::cout << std::endl << "Frame Hex Data:" << std::endl;
  for (int i = 0; i < RS_DECODED_SIZE; i++) {
    printf(" 0x%x", output[i]);
  }
  std::cout << std::endl << "End of Frame Hex Data" << std::endl;

  uint32_t bytesUnpacked = 0;
  while (bytesUnpacked < AES_DECRYPTED_SIZE) {
    if (output[bytesUnpacked] == CMD_END_OF_FRAME) {
      // means we have reached the end of the packet and rest can be ignored
      break;
    }
    cmd_msg_t command;
    EXPECT_EQ(unpackCmdMsg(output, &bytesUnpacked, &command), OBC_GS_ERR_CODE_SUCCESS);
    printf(" Command id: %d", command.id);
    printf(" Offset: %d", bytesUnpacked);
  }
  std::cout << std::endl;
}

TEST(TestEncodeDecode, sendCommandData) {
  uint8_t data[RS_DECODED_SIZE] = {0};
  cmd_msg_t cmdReset = {0};
  cmdReset.id = CMD_EXEC_OBC_RESET;

  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  ASSERT_EQ(packCmdMsg(data, &packOffset, &cmdReset, &numPacked), OBC_GS_ERR_CODE_SUCCESS);

  cmd_msg_t cmdPing = {0};
  cmdPing.id = CMD_PING;
  ASSERT_EQ(packCmdMsg(data, &packOffset, &cmdPing, &numPacked), OBC_GS_ERR_CODE_SUCCESS);

  setCurrentLinkDestCallSign((uint8_t *)"AKITO", CALLSIGN_LENGTH, DEFAULT_SSID);

  // Create fec data and encode
  packed_rs_packet_t fecData = {0};
  ASSERT_EQ(rsEncode(data, &fecData), OBC_GS_ERR_CODE_SUCCESS);

  // Create ax25 Frame
  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  ASSERT_EQ(ax25SendIFrame((uint8_t *)&fecData.data, RS_ENCODED_SIZE, &unstuffedAx25Data), OBC_GS_ERR_CODE_SUCCESS);

  // Stuff the ax25 Frame
  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, (uint8_t *)ax25Data.data, &ax25Data.length),
            OBC_GS_ERR_CODE_SUCCESS);

  // Manipulate some bits for testing purposes
  ax25Data.data[40] ^= 0b10000001;
  ax25Data.data[220] ^= 0b10100011;

  // Print out the ax25 data in hex to use in python testing
  // NOTE: Uncomment to see output when running c tests
  std::cout << std::endl << "Frame Hex Data:" << std::endl;
  for (int i = 0; i < ax25Data.length; i++) {
    printf(" 0x%x", ax25Data.data[i]);
  }
  std::cout << std::endl << "End of Frame Hex Data" << std::endl;

  // NOTE: This should only be called after everything with fec is done (thus why its not called in the first test)
  destroyRs();
}
