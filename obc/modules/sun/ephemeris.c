#include "ephermeris.h"

#include "sun_utils.h"
#include "sun_file.h"

#include <stdbool.h>
#include <os_semphr.h>
#include <FreeRTOS.h>
#include <string.h>

// Only 1 task can access the file at a time
static SemaphoreHandle_t fileMutex = NULL;
static StaticSemaphore_t fileBuffer;

static const char *fileName = "/sunData.bin";
static bool initialized = false;  // There's probably a better way to do this

void initSunPosition(void) {
  if (fileMutex == NULL) {
    fileMutex = xSemaphoreCreateBinaryStatic(&fileBuffer);
  }

  ASSERT(fileMutex != NULL);

  if (!initialized) {
    ASSERT(sunFileInit(fileName) == OBC_ERR_CODE_SUCCESS);
    initialized = true;
  }
}

obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer) {
  if (!initialized) return OBC_ERR_SUN_POSITION_NOT_INITIALIZED;
  if
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}
