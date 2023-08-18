#include "image_processing.h"

#include "obc_errors.h"
#include <gtest/gtest.h>

#include <stdint.h>
#include <stdlib.h>

TEST(TestObcImageProcessing, breakImageIntoPackets) {
  image_t image = {.width = 640, .height = 480, .data = new uint8_t[640 * 480]};

  uint32_t packetLength = 50;
  image_t *packets;
  uint32_t numPackets = breakImageIntoPackets(&image, &packets, packetLength);
  // 10 packets of 640x50 and 1 packet of 640x30
  EXPECT_EQ(numPackets, 10);
  for (uint32_t i = 0; i < numPackets - 1; i++) {
    EXPECT_EQ(packets[i].width, 640);
    EXPECT_EQ(packets[i].height, 50);
  }
  EXPECT_EQ(packets[numPackets - 1].width, 640);
  EXPECT_EQ(packets[numPackets - 1].height, 30);

  delete packets;
  delete image.data;
  packets = NULL;
  image.data = NULL;
}

TEST(TestObcImageProcessing, findBrightestPixelInPacket) {
  image_t packet = {.width = 640, .height = 50, .data = new uint8_t[640 * 50]};

  // fill up the packet with random data and set one pixel to 255
  for (uint32_t i = 0; i < packet.width * packet.height; i++) {
    packet.data[i] = rand() % 255;
  }
  packet.data[320 + 25 * packet.width] = 255;

  uint8_t brightness = 0;
  uint32_t brightestX = 0;
  uint32_t brightestY = 0;
  findBrightestPixelInPacket(&packet, &brightestX, &brightestY, &brightness, 0);
  EXPECT_EQ(brightness, 255);
  EXPECT_EQ(brightestX, 320);
  EXPECT_EQ(brightestY, 25);
}

TEST(TestObcImageProcessing, findBrightestPixel) {
  image_t image = {.width = 640, .height = 480, .data = new uint8_t[640 * 480]};

  // fill up the image with random data and set one pixel to 255
  for (uint32_t i = 0; i < image.width * image.height; i++) {
    image.data[i] = rand() % 255;
  }
  image.data[320 + 240 * image.width] = 255;

  uint32_t brightestX = 0;
  uint32_t brightestY = 0;
  findBrightestPixel(&image, &brightestX, &brightestY);
  EXPECT_EQ(brightestX, 320);
  EXPECT_EQ(brightestY, 240);

  delete image.data;
  image.data = NULL;
}
