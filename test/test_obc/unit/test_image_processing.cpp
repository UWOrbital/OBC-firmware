#include "image_processing.h"

#include "obc_errors.h"
#include <gtest/gtest.h>

#include <stdint.h>
#include <stdlib.h>

TEST(TestObcImageProcessing, findBrightestPixelInPacket) {
  image_t image = {640, 480, (uint8_t[640 * 480]){0}};

  image_t topPacket = {640, 240, image.data};
  image_t bottomPacket = {640, 240, image.data + 640 * 240};

  // set image data to random values < 254
  for (uint32_t i = 0; i < image.width * image.height; i++) {
    image.data[i] = rand() % 254;
  }

  image.data[20 + 40 * image.width] = 254;
  image.data[560 + 400 * image.width] = 255;

  /**
   * we expect the brightest pixel in the top packet to be at (20, 40) with brightness 254
   * and the brightest pixel in the bottom packet to be at (560, 400) with brightness 255
   * overall, the brightest pixel should be at (560, 400) with brightness 255
   */
  uint16_t brightestX = 0;
  uint16_t brightestY = 0;
  uint8_t brightess = 0;
  findBrightestPixelInPacket(&topPacket, &brightestX, &brightestY, &brightess, 0);
  EXPECT_EQ(brightestX, 20);
  EXPECT_EQ(brightestY, 40);
  EXPECT_EQ(brightess, 254);
  findBrightestPixelInPacket(&bottomPacket, &brightestX, &brightestY, &brightess, 240);
  EXPECT_EQ(brightestX, 560);
  EXPECT_EQ(brightestY, 400);
  EXPECT_EQ(brightess, 255);
}
