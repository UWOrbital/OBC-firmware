#include "vn100_common.h"
#include "obc_sci_io.h"
#include "obc_errors.h"
#include "obc_gs_crc.h"

#include <stdint.h>
#include <stdlib.h>

#include <gtest/gtest.h>

TEST(TestVn100PackUnpack, ValidVn100PackUnpack) {
  obc_error_code_t err;

  const uint8_t header [] = {0xFA, 0x01, 0x0528};
  const float payload [14] = {35.0300, 32.9274, 73.3720, 7.0980, 39.3274, 16.7880, 84.7893, 40.1305, 82.1947, 89.9776, 57.8206, 47.1948, 81.2268, 26.0413}; 
  uint16_t crc = calculateCrc16Ccitt(&payload, sizeof(payload));

  // Calculate the total size of the data array
  size_t totalSize = sizeof(header) + sizeof(payload) + sizeof(crc);

  // Create a char array to store the combined data
  unsigned char mockVn100Packet[totalSize];

  // Copy the header, payload, and crc into the combinedData array
  memcpy(mockVn100Packet, header, sizeof(header));
  memcpy(mockVn100Packet + sizeof(header), payload, sizeof(payload));
  memcpy(mockVn100Packet + sizeof(header) + sizeof(payload), &crc, sizeof(crc));

  initVn100();

  vn100_binary_packet_t receivedPacket;

  ASSERT_EQ(readBinaryOutputs(&receivedPacket), OBC_ERR_CODE_SUCCESS); 
  EXPECT_EQ(receivedPacket.yaw, payload[0]);
  EXPECT_EQ(receivedPacket.pitch, payload[1]);
  EXPECT_EQ(receivedPacket.roll, payload[2]);

  EXPECT_EQ(receivedPacket.gyroX, payload[3]);
  EXPECT_EQ(receivedPacket.gyroY, payload[4]);
  EXPECT_EQ(receivedPacket.gyroZ, payload[5]);

  EXPECT_EQ(receivedPacket.accelX, payload[6]);
  EXPECT_EQ(receivedPacket.accelY, payload[7]);
  EXPECT_EQ(receivedPacket.accelZ, payload[8]);

  EXPECT_EQ(receivedPacket.magX, payload[9]);
  EXPECT_EQ(receivedPacket.magY, payload[10]);
  EXPECT_EQ(receivedPacket.magX, payload[11]);

  EXPECT_EQ(receivedPacket.temp, payload[12]);
  EXPECT_EQ(receivedPacket.pres, payload[13]);

}