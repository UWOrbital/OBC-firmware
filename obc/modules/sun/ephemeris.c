#include "ephemeris.h"

#include "obc_errors.h"
#include "obc_logging.h"
#include "sun_utils.h"
#include "sun_file.h"
#include "sys_common.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <string.h>
#include <stdbool.h>

// Only 1 task can access the file at a time
static SemaphoreHandle_t fileMutex = NULL;
static StaticSemaphore_t fileBuffer;

static const char *fileName = "/sunData.bin";
static bool initialized = false;  // There's probably a better way to do this

obc_error_code_t initSunPosition(void) {
  if (fileMutex == NULL) {
    fileMutex = xSemaphoreCreateBinaryStatic(&fileBuffer);
  }

  ASSERT(fileMutex != NULL);

  if (!initialized) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(sunFileInit(fileName));
    initialized = true;
  }
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer) {
  if (!initialized) return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
  if (buffer == NULL) return OBC_ERR_CODE_INVALID_ARG;

  bool isInRange = false;
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sunFileJDInRange(jd, &isInRange));
  if (!isInRange) return OBC_ERR_CODE_SUN_POSITION_JD_OUT_OF_RANGE;

  // Read data point
  uint32_t index;
  position_data_t dataLower;
  RETURN_IF_ERROR_CODE(sunFileGetIndexOfJD(jd, &index));
  RETURN_IF_ERROR_CODE(sunFileReadDataPoint(index, &dataLower));

  if (doubleCloseDefault((double)jd, (double)dataLower.julianDate)) {
    memcpy(buffer, &dataLower, sizeof(position_data_t));
    return OBC_ERR_CODE_SUCCESS;
  }

  // Handle interpolation
  position_data_t dataHigher;
  RETURN_IF_ERROR_CODE(sunFileReadDataPoint(index + 1, &dataHigher));
  position_data_t newData = {jd, 0, 0, 0};
  RETURN_IF_ERROR_CODE(
      linearlyInterpolate(jd, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate, &(newData.x)));
  RETURN_IF_ERROR_CODE(
      linearlyInterpolate(jd, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate, &(newData.y)));
  RETURN_IF_ERROR_CODE(
      linearlyInterpolate(jd, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate, &(newData.z)));
  memcpy(buffer, &newData, sizeof(position_data_t));

  return OBC_ERR_CODE_SUCCESS;
}
