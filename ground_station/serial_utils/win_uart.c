#include "win_uart.h"

#include <windows.h>
#include <stdint.h>

int openSerialPort(HANDLE *hSerial, char *portName) {
  if (hSerial == NULL || portName == NULL) {
    return -1;
  }

  *hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (*hSerial == INVALID_HANDLE_VALUE) {
    return -1;
  }

  return 0;
}

int closeSerialPort(HANDLE hSerial) {
  if (CloseHandle(hSerial) == 0) {
    return -1;
  }

  return 0;
}

int getDeviceState(HANDLE hSerial, DCB *dcbSerialParams) {
  if (GetCommState(hSerial, dcbSerialParams) == 0) {
    CloseHandle(hSerial);
    return 1;
  }

  return 0;
}

int setDeviceParameters(HANDLE hSerial, DCB *dcbSerialParams) {
  if (SetCommState(hSerial, dcbSerialParams) == 0) {
    CloseHandle(hSerial);
    return -1;
  }

  return 0;
}

int setCommTimeouts(HANDLE hSerial, COMMTIMEOUTS *timeouts) {
  if (SetCommTimeouts(hSerial, timeouts) == 0) {
    CloseHandle(hSerial);
    return -1;
  }

  return 0;
}

long unsigned int writeSerialPort(HANDLE hSerial, uint8_t *buff, size_t len) {
  long unsigned int bytesWritten = 0;
  if (!WriteFile(hSerial, buff, len, &bytesWritten, NULL)) {
    CloseHandle(hSerial);
    return 0;
  }

  return bytesWritten;
}

int readSerialPort(HANDLE hSerial, uint8_t *buff, size_t len) {
  long unsigned int bytesRead = 0;
  if (!ReadFile(hSerial, buff, len, &bytesRead, NULL)) {
    CloseHandle(hSerial);
    return -1;
  }

  return bytesRead;
}
