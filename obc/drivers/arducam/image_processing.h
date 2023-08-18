#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A struct to store an image
 * @param width The width of the image
 * @param height The height of the image
 * @param data The image data
 */
typedef struct image_t {
  uint32_t width;
  uint32_t height;
  uint8_t *data;
} image_t;

/**
 * @brief Break an image into packets, cannot process the entire image at once due to memory constraints
 * @param image The image to break into packets
 * @param packets The array to store the packets in
 * @param packetHeight The height of each packet, the width of each packet is the same as the image
 * @return The number of packets created
 */
uint32_t breakImageIntoPackets(image_t *image, image_t **packets, uint32_t packetHeight);

/**
 * @brief Find the brightest pixel in an image
 * @param packet The image packet to search through
 * @param x The x coordinate of the brightest pixel
 * @param y The y coordinate of the brightest pixel
 * @param threshold Brightest pixel found so far, used to speed up search to only look at pixels brighter than this
 * @param packetStartingY The y coordinate of the top left corner of the packet, used to calculate the actual y
 * coordinate of the brightest pixel
 */
void findBrightestPixelInPacket(image_t *packet, uint32_t *x, uint32_t *y, uint8_t *brightness,
                                uint32_t packetStartingY);

/**
 * @brief Find the brightest pixel in an image
 * @param image The image to search through
 * @param x The x coordinate of the brightest pixel
 * @param y The y coordinate of the brightest pixel
 */
void findBrightestPixel(image_t *image, uint32_t *x, uint32_t *y);

#ifdef __cplusplus
}
#endif
