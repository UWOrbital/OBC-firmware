#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"
#include "obc_i2c_io.h"
#include "obc_print.h"

#include "arducam.h"
#include "camera_control.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <i2c.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY
#define BUFFER_SIZE 512U

#define TASK_STACK_SIZE 2048U
static StaticTask_t taskBuffer;
static StackType_t taskStack[TASK_STACK_SIZE];

void vTask1(void *pvParameters) {
  sciPrintf("Starting Arducam Demo\r\n");
  selectCamera(PRIMARY);
  initCamera();
  uint8_t temp;
  arducamReadSensorPowerControlReg(&temp);
  sciPrintf("Power Control Reg:0x%X\r\n", temp);

  // Read Camera Sensor ID
  uint8_t cam_id[2] = {0};
  camReadSensorReg16_8(0x300A, &cam_id[0]);
  camReadSensorReg16_8(0x300B, &cam_id[1]);
  sciPrintf("Sensor ID: %X%X\r\n", cam_id[0], cam_id[1]);

  // Test Reg operations
  uint8_t byte = 0x55;
  sciPrintf("Writing %d to test reg\r\n", byte);
  arducamWriteTestReg(byte);
  byte = 0;
  arducamReadTestReg(&byte);
  sciPrintf("Read %d from test reg\r\n", byte);

  // Camera Configuration
  sciPrintf("Configuring Camera\r\n");
  camConfigureSensor();

  // Capture
  sciPrintf("Starting Image Capture\r\n");
  startImageCapture();
  while (!isCaptureDone())
    ;
  sciPrintf("Image Capture Done ^_^\r\n");

  // Read image size
  uint32_t img_len = 0;
  arducamReadFIFOSize(&img_len);
  sciPrintf("image len: %d \r\n", img_len);

  // Read image from FIFO
  uint8_t imgBuffer[BUFFER_SIZE];
  sciPrintf("FIFO Burst Read:\r\n");
  size_t bytesRead = 0;
  obc_error_code_t ret;
  do {
    ret = readImage(imgBuffer, BUFFER_SIZE, &bytesRead);
    for (size_t index = 0; index < bytesRead; index++) {
      sciPrintf("0x%X,", imgBuffer[index]);
    }
  } while (ret == OBC_ERR_CODE_CAMERA_IMAGE_READ_INCOMPLETE);

  // Put Camera on standby (gets pretty hot if left powered on for too long)
  standbyCamera();
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

  xTaskCreateStatic(vTask1, "Arducam", TASK_STACK_SIZE, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;

  return 0;
}
