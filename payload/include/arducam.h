#include <stdint.h>

#define BMP 	0
#define JPEG	1
#define RAW	  2

#define OV5642_320x240 		0	// 320x240
#define OV5642_640x480		1	// 640x480
#define OV5642_1024x768		2	// 1024x768
#define OV5642_1280x960 	3	// 1280x960
#define OV5642_1600x1200	4	// 1600x1200
#define OV5642_2048x1536	5	// 2048x1536
#define OV5642_2592x1944	6	// 2592x1944
#define OV5642_1920x1080  7

#define ARDUCHIP_TIM       		0x03  // Timming control
#define VSYNC_LEVEL_MASK   		0x02  // 0 = High active , 		1 = Low active
#define ARDUCHIP_FRAMES			  0x01  // FRAME control register, Bit[2:0] = Number of frames to be captured  //  On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full

#define ARDUCHIP_FIFO      		0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02

#define BURST_FIFO_READ			0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ		0x3D  //Single FIFO read operation

#define FIFO_SIZE1				0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  //Camera write FIFO size[18:16]

void set_format(uint8_t fmt);
void InitCAM();
void OV5642_set_JPEG_size(uint8_t size);

void set_fifo_burst();
void flush_fifo();
void start_capture();
void clear_fifo_flag();
uint32_t read_fifo_length();
uint8_t read_fifo();
uint8_t read_reg(uint8_t addr);
void write_reg(uint8_t addr, uint8_t data);
void set_bit(uint8_t addr, uint8_t bit);
void clear_bit(uint8_t addr, uint8_t bit);
uint8_t get_bit(uint8_t addr, uint8_t bit);
uint8_t bus_write(int address,int value);
uint8_t bus_read(int address);


