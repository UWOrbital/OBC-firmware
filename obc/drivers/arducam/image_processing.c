#include "image_processing.h"

#include <stdlib.h>

/**
 * @brief Break an image into packets, cannot process the entire image at once due to memory constraints
 * @param image The image to break into packets
 * @param packets The array to store the packets in
 * @param packetLength The height of each packet, the width of each packet is the same as the image
 * @return The number of packets created
 */
uint32_t breakImageIntoPackets(image_t *image, image_t **packets, uint32_t packetHeight) {
  uint32_t num_chunks = 0;
  uint32_t total_chunks = image->height / packetHeight + (image->height % packetHeight > 0);

  *packets = malloc(total_chunks * sizeof(image_t));

  for (uint32_t i = 0; i < total_chunks; i++) {
    (*packets)[i].width = image->width;
    // the last packet may be smaller than the rest if the image height is not a multiple of the packet height
    (*packets)[i].height = (i < total_chunks - 1) ? packetHeight : (image->height - packetHeight * (total_chunks - 1));
    (*packets)[i].data = image->data + i * image->width * packetHeight;
    num_chunks++;
  }

  return num_chunks;
}

/**
 * @brief Find the brightest pixel in an image
 * @param packet The image packet to search through
 * @param x The x coordinate of the brightest pixel
 * @param y The y coordinate of the brightest pixel
 * @param threshold Brightest pixel found so far, used to speed up search by only looking at pixels brighter than this
 * @param packetStartingY The y coordinate of the top left corner of the packet, used to calculate the actual y
 * coordinate of the brightest pixel
 */
void findBrightestPixelInPacket(image_t *packet, uint32_t *x, uint32_t *y, uint8_t *brightness,
                                uint32_t packetStartingY) {
  for (uint32_t i = 0; i < packet->height; i++) {
    for (uint32_t j = 0; j < packet->width; j++) {
      if (packet->data[i * packet->width + j] > *brightness) {
        *brightness = packet->data[i * packet->width + j];
        *x = j;
        *y = packetStartingY + i;  // need to account for packet position within the image
      }
    }
  }
}

/**
 * @brief Find the brightest pixel in an image
 * @param image The image to search through
 * @param x The x coordinate of the brightest pixel
 * @param y The y coordinate of the brightest pixel
 */
void findBrightestPixel(image_t *image, uint32_t *x, uint32_t *y) {
  uint32_t num_chunks = 0;
  uint32_t packetLength = 100;
  image_t *packets;

  num_chunks = breakImageIntoPackets(image, &packets, packetLength);

  uint8_t brightness = 0;
  uint32_t brightestX = 0;
  uint32_t brightestY = 0;

  for (uint32_t i = 0; i < num_chunks; i++) {
    findBrightestPixelInPacket(&packets[i], &brightestX, &brightestY, &brightness, i * packetLength);
  }

  *x = brightestX;
  *y = brightestY;

  free(packets);
}
