#include "bl_config.h"
#include "bl_flash.h"
#include "bl_uart.h"
#include "obc_errors.h"
#include "obc_reliance_fs.h"

#include <redposix.h>
#include <stdio.h>
#include <string.h>

/* LINKER EXPORTED SYMBOLS */
extern uint32_t __ramFuncsLoadStart__;
extern uint32_t __ramFuncsSize__;

extern uint32_t __ramFuncsRunStart__;
extern uint32_t __ramFuncsRunEnd__;

/* DEFINES */
#define SDC_APP_LOAD 1U  // Set to 1 to load app from SD without user interation, 0 to load app from UART
// These values were chosen so that the UART transfers and flash writes are quick, but don't
// use too much RAM
#define BL_BIN_RX_CHUNK_SIZE 128U   // Bytes
#define BL_ECC_FIX_CHUNK_SIZE 128U  // Bytes

/* TYPEDEFS */
typedef void (*appStartFunc_t)(void);

// If this header changes, update the host utility
typedef struct {
  uint32_t version;
  uint32_t size;
} app_header_t;

typedef enum {
  BL_STATE_IDLE,
  BL_STATE_DOWNLOAD_IMAGE,
  BL_STATE_ERASE_IMAGE,
  BL_STATE_RUN_APP,
  BL_STATE_WRITE_APP_TO_SD,
  BL_STATE_FLASH_FROM_SD,
} bl_state_t;

/* PRIVATE FUNCTIONS */
// TODO: Replace this with fs wrapper functions once we have a way to
// write to SD card from the app (current setupFileSystem fs wrapper function formats SD card)
void blSdcInit() {
  int32_t ret;

  ret = red_init();
  if (ret != 0) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("File system init FAILED.\r\n"),
                     (uint8_t *)"File system init FAILED.\r\n");
    while (1)
      ;
  }

  ret = red_mount("");
  if (ret != 0) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("File system mount FAILED.\r\n"),
                     (uint8_t *)"File system mount FAILED.\r\n");
    while (1)
      ;
  }
}

void blSdcDonwloadImage() {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;
  char *fname = "boot/app.out";  // Find
  obc_error_code_t ret;

  /* Open file */
  int32_t fileID = -1;
  ret = openFile(fname, &fileID, RED_O_RDONLY);

  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("File /app.out NO FOUND.\r\n"), (uint8_t *)"File /app.out NO FOUND.\r\n");
    while (1)
      ;
  }

  /* Read app and write to FLASH */
  uint8_t readBuf[sizeof(app_header_t)] = {0U};
  size_t bytesRead;
  ret = readFile(fileID, readBuf, sizeof(app_header_t), &bytesRead);
  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to read app header.\r\n"),
                     (uint8_t *)"Failed to read app header.\r\n");
    while (1)
      ;
  }
  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Read app header!\r\n"), (uint8_t *)"Read app header!\r\n");

  app_header_t appHeader = {0};
  memcpy((void *)&appHeader, (void *)readBuf, sizeof(app_header_t));

  if (appHeader.size == 0U) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Invalid image size\r\n"), (uint8_t *)"Invalid image size\r\n");
    while (1)
      ;
  }

  if (!blFlashIsStartAddrValid(APP_START_ADDRESS, appHeader.size)) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Invalid start address\r\n"), (uint8_t *)"Invalid start address\r\n");
    while (1)
      ;
  }

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Received header\r\n"), (uint8_t *)"Received header\r\n");

  /* Init Bank and Erase Flash block */
  errCode = blFlashFapiInitBank(0U);
  if (errCode != BL_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to init flash\r\n"), (uint8_t *)"Failed to init flash\r\n");
    while (1)
      ;
  }

  errCode = blFlashFapiBlockErase(APP_START_ADDRESS, appHeader.size);
  if (errCode != BL_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to erase flash\r\n"), (uint8_t *)"Failed to erase flash\r\n");
    while (1)
      ;
  }

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Erased flash\r\n"), (uint8_t *)"Erased flash\r\n");

  blFlashFapiInitBank(0U);

  // Receive image in chunks and write to flash
  uint32_t numAppBytesToFlash = appHeader.size;
  while (numAppBytesToFlash > 0) {
    uint8_t readBuf[BL_BIN_RX_CHUNK_SIZE] = {0U};

    uint32_t numBytesToRead = (numAppBytesToFlash > BL_BIN_RX_CHUNK_SIZE) ? BL_BIN_RX_CHUNK_SIZE : numAppBytesToFlash;

    ret = readFile(fileID, readBuf, numBytesToRead, &bytesRead);
    if (ret != OBC_ERR_CODE_SUCCESS) {
      blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to read app chunk.\r\n"),
                       (uint8_t *)"Failed to read app chunk.\r\n");
      while (1)
        ;
    }

    blFlashFapiBlockWrite(APP_START_ADDRESS + (appHeader.size - numAppBytesToFlash), (uint32_t)readBuf, numBytesToRead);
    numAppBytesToFlash -= numBytesToRead;
  }

  ret = closeFile(fileID);

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Wrote application\r\n"), (uint8_t *)"Wrote application\r\n");

  // Fix the ECC for any flash memory that was erased, but not overwritten by the new app
  uint8_t eccFixWriteBuf[BL_ECC_FIX_CHUNK_SIZE] = {0U};
  memset(eccFixWriteBuf, 0xFFU, sizeof(eccFixWriteBuf));  // Erased flash defaults to 0xFF

  const uint32_t eccFixTotalBytes = blFlashSectorEndAddr(blFlashSectorOfAddr(APP_START_ADDRESS + appHeader.size)) -
                                    (APP_START_ADDRESS + appHeader.size);

  uint32_t eccFixBytesLeft = eccFixTotalBytes;
  while (eccFixBytesLeft > 0) {
    const uint32_t numBytesToWrite =
        (eccFixBytesLeft > BL_ECC_FIX_CHUNK_SIZE) ? BL_ECC_FIX_CHUNK_SIZE : eccFixBytesLeft;

    const uint32_t baseAddr = APP_START_ADDRESS + appHeader.size + 1U;
    const uint32_t addr = baseAddr + (eccFixTotalBytes - eccFixBytesLeft);

    blFlashFapiBlockWrite(addr, (uint32_t)eccFixWriteBuf, numBytesToWrite);

    eccFixBytesLeft -= numBytesToWrite;
  }

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Finished writing to flash\r\n"),
                   (uint8_t *)"Finished writing to flash\r\n");
}

/* TMP function to receive app through UART and write to SD card */
/* TODO: Remove this function and all calls to it once we have a more concrete way of writing app to SD card */
static void writeAppToSD() {
  uint16_t ret = BL_ERR_CODE_UNKNOWN;

  ret = setupFileSystem();
  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to init file system.\r\n"),
                     (uint8_t *)"Failed to init file system.\r\n");
    return;
  }

  // Create 'images' folder if not exists
  char *imageDir = "boot";
  ret = mkDir(imageDir);
  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to create images diretory.\r\n"),
                     (uint8_t *)"Failed to create images diretory.\r\n");
    return;
  }

  // TODO: Dynamic file names based on app version
  char *imageName = "app.out";
  char filePath[64] = "";
  strcat(filePath, imageDir);
  strcat(filePath, "/");
  strcat(filePath, imageName);
  int32_t fileID = -1;
  ret = createFile(filePath, &fileID);
  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to file in SD.\r\n"), (uint8_t *)"Failed to file in SD.\r\n");
    return;
  }

  ret = openFile(filePath, &fileID, RED_O_WRONLY);
  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to open file.\r\n"), (uint8_t *)"Failed to open file.\r\n");
    return;
  }

  // Read app from UART and write to SD
  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Downloading application\r\n"), (uint8_t *)"Downloading application\r\n");

  uint8_t recvBuffer[sizeof(app_header_t)] = {0U};

  blUartReadBytes(BL_UART_SCIREG_2, recvBuffer, sizeof(app_header_t));

  app_header_t appHeader = {0};
  memcpy((void *)&appHeader, (void *)recvBuffer, sizeof(app_header_t));

  if (appHeader.size == 0U) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Invalid image size\r\n"), (uint8_t *)"Invalid image size\r\n");
    return;
  }

  if (!blFlashIsStartAddrValid(APP_START_ADDRESS, appHeader.size)) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Invalid start address\r\n"), (uint8_t *)"Invalid start address\r\n");
    return;
  }

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Received header. Writting header to SD...\r\n"),
                   (uint8_t *)"Received header. Writting header to SD...\r\n");

  ret = writeFile(fileID, recvBuffer, sizeof(app_header_t));
  if (ret != OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to write app header to SD.\r\n"),
                     (uint8_t *)"Failed to write app header to SD.\r\n");
    return;
  }

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Send D char before sending app.\r\n"),
                   (uint8_t *)"Send D char before sending app.\r\n");
  // Host will send a 'D' before sending the image
  while (1) {
    char waitChar = '\0';

    blUartReadBytes(BL_UART_SCIREG_2, (uint8_t *)&waitChar, 1U);

    if (waitChar == 'D') {
      break;
    }
  }

  // Receive image in chunks and write to SD
  uint32_t numAppBytesToFlash = appHeader.size;
  while (numAppBytesToFlash > 0) {
    uint8_t recvBuffer[BL_BIN_RX_CHUNK_SIZE] = {0U};

    uint32_t numBytesToRead = (numAppBytesToFlash > BL_BIN_RX_CHUNK_SIZE) ? BL_BIN_RX_CHUNK_SIZE : numAppBytesToFlash;

    blUartReadBytes(BL_UART_SCIREG_2, recvBuffer, numBytesToRead);

    writeFile(fileID, recvBuffer, numBytesToRead);  // Write app chunk to SD

    numAppBytesToFlash -= numBytesToRead;
  }

  blUartWriteBytes(BL_UART_SCIREG_1, strlen("Wrote application to SD.\r\n"), (uint8_t *)"Wrote application to SD.\r\n");

  closeFile(fileID);
}

/* PUBLIC FUNCTIONS */
int main(void) {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;

  blUartInit();
  blSdcInit();

  // F021 API and the functions that use it must be executed from RAM since they
  // can't execute from the same flash bank being modified
  memcpy(&__ramFuncsRunStart__, &__ramFuncsLoadStart__, (uint32_t)&__ramFuncsSize__);

  if (SDC_APP_LOAD) {
    // This build flag should be set for SD app loads as UART is not going to be
    // used if bl is operating in this mode
    blSdcDonwloadImage();

    // Go to the application's entry point
    uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
    ((appStartFunc_t)appStartAddress)();

    blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to run application\r\n"),
                     (uint8_t *)"Failed to run application. Going into UART app load mode.\r\n");
  }

  bl_state_t state = BL_STATE_IDLE;
  // TODO: Modify the transfer protocol to be faster and more robust
  while (1) {
    switch (state) {
      case BL_STATE_IDLE: {
        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Waiting for input\r\n"), (uint8_t *)"Waiting for input\r\n");

        char c = '\0';
        blUartReadBytes(BL_UART_SCIREG_2, (uint8_t *)&c, 1);

        if (c == 'd') {
          state = BL_STATE_DOWNLOAD_IMAGE;
        } else if (c == 'e') {
          state = BL_STATE_ERASE_IMAGE;
        } else if (c == 'r') {
          state = BL_STATE_RUN_APP;
        } else if (c == 's') {
          state = BL_STATE_WRITE_APP_TO_SD;
        } else if (c == 'f') {
          state = BL_STATE_FLASH_FROM_SD;
        }

        break;
      }
      case BL_STATE_FLASH_FROM_SD: {
        // call flashing from SD
        blSdcDonwloadImage();

        // Go to the application's entry point
        uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
        ((appStartFunc_t)appStartAddress)();

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to run application. Going into UART app load mode.\r\n"),
                         (uint8_t *)"Failed to run application. Going into UART app load mode.\r\n");
        break;
      }

      case BL_STATE_WRITE_APP_TO_SD: {
        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Downloading application to SD card...\r\n"),
                         (uint8_t *)"Downloading application to SD card...\r\n");
        writeAppToSD();

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Downloaded app to SD. Send 'f' to flash app from SD.\r\n"),
                         (uint8_t *)"Downloaded app to SD. Send 'f' to flash app from SD.\r\n");

        break;
      }
      case BL_STATE_DOWNLOAD_IMAGE: {
        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Downloading application\r\n"),
                         (uint8_t *)"Downloading application\r\n");

        uint8_t recvBuffer[sizeof(app_header_t)] = {0U};

        blUartReadBytes(BL_UART_SCIREG_2, recvBuffer, sizeof(app_header_t));

        app_header_t appHeader = {0};
        memcpy((void *)&appHeader, (void *)recvBuffer, sizeof(app_header_t));

        if (appHeader.size == 0U) {
          blUartWriteBytes(BL_UART_SCIREG_1, strlen("Invalid image size\r\n"), (uint8_t *)"Invalid image size\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        if (!blFlashIsStartAddrValid(APP_START_ADDRESS, appHeader.size)) {
          blUartWriteBytes(BL_UART_SCIREG_1, strlen("Invalid start address\r\n"),
                           (uint8_t *)"Invalid start address\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Received header\r\n"), (uint8_t *)"Received header\r\n");

        errCode = blFlashFapiInitBank(0U);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to init flash\r\n"), (uint8_t *)"Failed to init flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        errCode = blFlashFapiBlockErase(APP_START_ADDRESS, appHeader.size);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to erase flash\r\n"),
                           (uint8_t *)"Failed to erase flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Erased flash\r\n"), (uint8_t *)"Erased flash\r\n");

        // Host will send a 'D' before sending the image
        while (1) {
          char waitChar = '\0';

          blUartReadBytes(BL_UART_SCIREG_2, (uint8_t *)&waitChar, 1U);

          if (waitChar == 'D') {
            break;
          }
        }

        blFlashFapiInitBank(0U);

        // Receive image in chunks and write to flash
        uint32_t numAppBytesToFlash = appHeader.size;
        while (numAppBytesToFlash > 0) {
          uint8_t recvBuffer[BL_BIN_RX_CHUNK_SIZE] = {0U};

          uint32_t numBytesToRead =
              (numAppBytesToFlash > BL_BIN_RX_CHUNK_SIZE) ? BL_BIN_RX_CHUNK_SIZE : numAppBytesToFlash;

          blUartReadBytes(BL_UART_SCIREG_2, recvBuffer, numBytesToRead);

          blFlashFapiBlockWrite(APP_START_ADDRESS + (appHeader.size - numAppBytesToFlash), (uint32_t)recvBuffer,
                                numBytesToRead);

          numAppBytesToFlash -= numBytesToRead;
        }

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Wrote application\r\n"), (uint8_t *)"Wrote application\r\n");

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Fixing ECC\r\n"), (uint8_t *)"Fixing ECC\r\n");

        // Fix the ECC for any flash memory that was erased, but not overwritten by the new app
        uint8_t eccFixWriteBuf[BL_ECC_FIX_CHUNK_SIZE] = {0U};
        memset(eccFixWriteBuf, 0xFFU, sizeof(eccFixWriteBuf));  // Erased flash defaults to 0xFF

        const uint32_t eccFixTotalBytes =
            blFlashSectorEndAddr(blFlashSectorOfAddr(APP_START_ADDRESS + appHeader.size)) -
            (APP_START_ADDRESS + appHeader.size);

        uint32_t eccFixBytesLeft = eccFixTotalBytes;
        while (eccFixBytesLeft > 0) {
          const uint32_t numBytesToWrite =
              (eccFixBytesLeft > BL_ECC_FIX_CHUNK_SIZE) ? BL_ECC_FIX_CHUNK_SIZE : eccFixBytesLeft;

          const uint32_t baseAddr = APP_START_ADDRESS + appHeader.size + 1U;
          const uint32_t addr = baseAddr + (eccFixTotalBytes - eccFixBytesLeft);

          blFlashFapiBlockWrite(addr, (uint32_t)eccFixWriteBuf, numBytesToWrite);

          eccFixBytesLeft -= numBytesToWrite;
        }

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Finished writing to flash\r\n"),
                         (uint8_t *)"Finished writing to flash\r\n");

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_ERASE_IMAGE: {
        blUartWriteBytes(BL_UART_SCIREG_1, strlen("NOT IMPLEMENTED\r\n"), (uint8_t *)"NOT IMPLEMENTED\r\n");

        // TODO: Erase entire application space

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_RUN_APP: {
        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Running application\r\n"), (uint8_t *)"Running application\r\n");

        // Go to the application's entry point
        uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
        ((appStartFunc_t)appStartAddress)();

        blUartWriteBytes(BL_UART_SCIREG_1, strlen("Failed to run application\r\n"),
                         (uint8_t *)"Failed to run application\r\n");

        // TODO: Restart device if application fails to run or returns

        break;
      }
    }
  }
}
