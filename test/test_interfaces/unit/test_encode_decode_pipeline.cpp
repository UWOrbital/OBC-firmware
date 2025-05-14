#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <math.h>
#include <string.h>

#include <gtest/gtest.h>
#include <stdio.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
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
  aes128Decrypt(&aesData, output, RS_DECODED_SIZE);

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

  // Write to file making sure that only the final frame gets written
  std::fstream writeFile("test.bin", std::ios::binary | std::ios::out);
  ASSERT_EQ(writeFile.is_open(), true);
  writeFile.clear();
  writeFile.write((char *)(&ax25Data.data[0]), sizeof(uint8_t) * ax25Data.length);
  std::remove("encode.bin");
  std::rename("test.bin", "encode.bin");
  writeFile.close();
}

// TEST: A simulated receive with the entire pipline
TEST(TestEncodeDecode, receiveData) {
  // Open file to read from
  std::fstream readFile("encode_more.bin", std::ios::in | std::ios::binary);
  ASSERT_EQ(readFile.is_open(), true);
  int file_length = 0;

  // Check the file length to know how much data we have to write
  readFile.seekg(0, std::ios::end);      // Move to the end of the file
  uint16_t fileSize = readFile.tellg();  // Get the current position (file size)
  readFile.seekg(0, std::ios::beg);      // Move back to the beginning of the file

  // Create a ax25 packed frame with the length of the file
  packed_ax25_i_frame_t ax25Data = {.length = fileSize};
  readFile.read((char *)(&ax25Data.data[0]), sizeof(uint8_t) * ax25Data.length);

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
  std::cout << std::endl;

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
    ASSERT_EQ(output[i], expected_data[i]);
  }
  readFile.close();

  // NOTE: This should only be called after everything with fec is done (thus why its not called in the first test)
  destroyRs();
}
