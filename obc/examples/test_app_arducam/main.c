#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"
#include "obc_i2c_io.h"
#include "obc_print.h"

#include "arducam.h"
#include "camera_reg.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <i2c.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

obc_error_code_t readFifoBurst2(uint32_t length) {
  obc_error_code_t errCode;
  uint8_t temp = 0, temp_last = 0;

  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, 1));

  // Set fifo to burst mode, receive continuous data until EOF
  errCode = camWriteByte(0x3C, PRIMARY);
  while (length-- && !errCode) {
    temp_last = temp;
    errCode = camReadByte(&temp, PRIMARY);
    sciPrintf("0x%X,", temp);
    if ((temp == 0xD9) && (temp_last == 0xFF)) {
      break;
    }
    if (length % 30 == 0) {
      sciPrintf("\r\n", temp);
    }
    // // Todo: Can this be changed to ~15us instead?
    // vTaskDelay(pdMS_TO_TICKS(1));
  }

  if (!errCode) {
    errCode = deassertChipSelect(CAM_SPI_PORT, 1);
  } else {
    // If there was an error during capture, deassert without an error check
    deassertChipSelect(CAM_SPI_PORT, 1);
  }

  return errCode;
}

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
  sciPrintf("Starting Arducam Demo\r\n");
  // Read Camera ID
  uint8_t cam_id[2] = {0};
  camReadSensorReg16_8(0x300A, &cam_id[0]);
  camReadSensorReg16_8(0x300B, &cam_id[1]);
  sciPrintf("Sensor ID: %X%X\r\n", cam_id[0], cam_id[1]);
  camWriteReg(0x07, 0x80, PRIMARY);
  vTaskDelay(pdMS_TO_TICKS(2));
  camWriteReg(0x07, 0x00, PRIMARY);
  vTaskDelay(pdMS_TO_TICKS(2));
  uint8_t byte = 0x55;
  sciPrintf("Writing %d to test reg\r\n", byte);
  camWriteReg(0x00, byte, PRIMARY);
  byte = 0;
  camReadReg(0x00, &byte, PRIMARY);
  sciPrintf("Read %d from test reg\r\n", byte);
  camReadReg(0x00, &byte, PRIMARY);
  sciPrintf("Read %d from test reg\r\n", byte);
  setFormat(JPEG);
  initCam();
  // camWriteReg(0x03, 0x02, PRIMARY);
  // camWriteSensorReg16_8(0x503d , 0x80);
  // camWriteSensorReg16_8(0x503e, 0x00);
  ov5642SetJpegSize(OV5642_320x240);
  vTaskDelay(pdMS_TO_TICKS(2));

  sciPrintf("Starting Image Capture\r\n");
  captureImage(PRIMARY);
  while (!isCaptureDone(PRIMARY))
    ;
  sciPrintf("Image Capture Done ^_^\r\n");

  uint32_t img_len = 0;
  readFifoLength(&img_len, PRIMARY);
  sciPrintf("image len: %d \r\n", img_len);

  uint32_t first_half = img_len / 2;
  uint32_t second_half = img_len - first_half;

  readFifoBurst2(first_half);
  readFifoLength(&img_len, PRIMARY);
  sciPrintf("image len: %d \r\n", img_len);
  readFifoBurst2(second_half);

  while (1)
    ;
}
int main(void) {
  // Initialize hardware.
  gioInit();
  sciInit();
  spiInit();
  i2cInit();

  // Initialize the bus mutex.
  initSciPrint();
  initSpiMutex();
  initI2CMutex();

  xTaskCreateStatic(vTask1, "Arducam", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;

  return 0;
}
