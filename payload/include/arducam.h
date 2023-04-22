#ifndef PAYLOAD_INCLUDE_ARDUCAM_H_
#define PAYLOAD_INCLUDE_ARDUCAM_H_

#include <stdint.h>
#include <stdbool.h>

// Image format
#define BMP 	0
#define JPEG	1
#define RAW	  2

// Image resolution
#define OV5642_320x240 		0
#define OV5642_640x480		1
#define OV5642_1024x768		2
#define OV5642_1280x960 	3
#define OV5642_1600x1200	4
#define OV5642_2048x1536	5
#define OV5642_2592x1944	6
#define OV5642_1920x1080  7

#define ARDUCHIP_TRIG      		0x41  // Trigger source
#define ARDUCHIP_TIM       		0x03  // Timing control
#define VSYNC_LEVEL_MASK   		0x02  // 0 = High active , 		1 = Low active
#define ARDUCHIP_FRAMES			  0x01  // FRAME control register, Bit[2:0] = Number of frames to be captured  //  On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#define CAP_DONE_MASK      		0x08
#define BURST_FIFO_READ			  0x3C  //Burst FIFO read operation

#define ARDUCHIP_FIFO      		0x04  // FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02
#define MAX_FIFO_SIZE		      0x7FFFFF  // 8MByte

#define FIFO_SIZE1				0x42  // Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  // Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  // Camera write FIFO size[18:16]

/**
 * @brief Set format to JPEG, BMP, or RAW
 * @param fmt The image format to set
 */
void set_format(uint8_t fmt);

/**
 * @brief Initialize camera
 */
void InitCAM(void);

/**
 * @brief Change JPEG resolution
 * @param size The JPEG resolution to set
 */
void OV5642_set_JPEG_size(uint8_t size);

/**
 * @brief Trigger an image capture
 */
void capture_image();

/**
 * @brief Checks if image capture has been completed
 * @return Returns true if capture is complete
 */
bool is_capture_done();

#endif /* PAYLOAD_INCLUDE_ARDUCAM_H_ */