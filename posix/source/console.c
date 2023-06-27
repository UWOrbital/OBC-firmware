#include <stdarg.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <os_semphr.h>

SemaphoreHandle_t xStdioMutex;

void initConsole(void) { xStdioMutex = xSemaphoreCreateMutex(); }

void printConsole(const char* fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  xSemaphoreTake(xStdioMutex, portMAX_DELAY);
  vprintf(fmt, vargs);
  xSemaphoreGive(xStdioMutex);
  va_end(vargs);
}
