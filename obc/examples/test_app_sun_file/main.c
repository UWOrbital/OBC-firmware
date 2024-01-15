#include "sun_file.h"

#include "obc_errors.h"
#include "obc_print.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>
#include <spi.h>
#include <can.h>
#include <het.h>

#include <string.h>
static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

#define STOP_ON_ERROR(text, _ret)                   \
  errCode = _ret;                                   \
  if (errCode != OBC_ERR_CODE_SUCCESS) {            \
    sciPrintf("%s returned %d\r\n", text, errCode); \
    while (1)                                       \
      ;                                             \
  }

void vTask1(void *pvParameters) {
  // Data from JD1 to JD99 inclusive with step of 1 JD
  const char *fName = "/sunData.bin";

  // Init

  obc_error_code_t errCode;
  STOP_ON_ERROR("sunFileInit", sunFileInit(fName));
  sciPrintf("Sun module successfully initialized");

  // Basic checks

  julian_date_t minJD;
  STOP_ON_ERROR("sunFileGetMinJD", sunFileGetMinJD(&minJD));
  sciPrintf("Minimum JD: %lf\r\n", minJD);

  julian_date_t maxJD;
  STOP_ON_ERROR("sunFileGetMaxJD", sunFileGetMaxJD(&maxJD));
  sciPrintf("Maximum JD: %lf\r\n", maxJD);

  uint32_t number;
  STOP_ON_ERROR("sunFileGetNumDataPoints", sunFileGetNumDataPoints(&number));
  sciPrintf("Number of data points: %d\r\n", number);

  // In Range Check

  bool isInRange = false;
  STOP_ON_ERROR("sunFileJDInRange JD=0.5", sunFileJDInRange(0.5, &isInRange));
  sciPrintf("isInRange: %d\r\n", isInRange);

  STOP_ON_ERROR("sunFileJDInRange JD=1", sunFileJDInRange(1, &isInRange));
  sciPrintf("isInRange: %d\r\n", isInRange);

  STOP_ON_ERROR("sunFileJDInRange JD=8", sunFileJDInRange(8, &isInRange));
  sciPrintf("isInRange: %d\r\n", isInRange);

  STOP_ON_ERROR("sunFileJDInRange JD=99", sunFileJDInRange(99, &isInRange));
  sciPrintf("isInRange: %d\r\n", isInRange);

  STOP_ON_ERROR("sunFileJDInRange JD=100", sunFileJDInRange(100, &isInRange));
  sciPrintf("isInRange: %d\r\n", isInRange);

  STOP_ON_ERROR("sunFileJDInRange JD=2.5", sunFileJDInRange(2.5, &isInRange));
  sciPrintf("isInRange: %d\r\n", isInRange);

  // Get Index of JD:

  uint32_t index;
  STOP_ON_ERROR("sunFileGetIndexOfJD JD=1", sunFileGetIndexOfJD(1, &index));
  sciPrintf("Index of JD=1 %d\r\n", index);

  STOP_ON_ERROR("sunFileGetIndexOfJD JD=33", sunFileGetIndexOfJD(33, &index));
  sciPrintf("Index of JD=33 %d\r\n", index);

  STOP_ON_ERROR("sunFileGetIndexOfJD JD=99", sunFileGetIndexOfJD(99, &index));
  sciPrintf("Index of JD=99 %d\r\n", index);

  // Should round down
  STOP_ON_ERROR("sunFileGetIndexOfJD JD=1.5", sunFileGetIndexOfJD(1.5, &index));
  sciPrintf("Index of JD=1.5 %d\r\n", index);

  STOP_ON_ERROR("sunFileGetIndexOfJD JD=50.4", sunFileGetIndexOfJD(50.4, &index));
  sciPrintf("Index of JD=50.4 %d\r\n", index);

  // Read data point

  position_data_t readData;

  // Stored at index 0:
  position_data_t expected1 = {
      .julianDate = 1, .x = 1.384519786747137E+08, .y = -5.472710939424842E+07, .z = -1.276932755237378E+06};
  STOP_ON_ERROR("sunFileReadDataPoint index=0", sunFileReadDataPoint(0, &readData));
  sciPrintf("Expected: ", closePositionData(expected1, readData));

  // Stored at index 32:
  position_data_t expected2 = {
      .julianDate = 33.000000000, .x = 1.481637529364224E+08, .y = 2.596441718873117E+07, .z = -5.378180895996094E+04};
  STOP_ON_ERROR("sunFileReadDataPoint index=33", sunFileReadDataPoint(33, &readData));
  sciPrintf("Expected: ", closePositionData(expected2, readData));

  // Stored at index 98:
  position_data_t expected3 = {
      .julianDate = 99.000000000, .x = 4.391066938786460E+07, .y = 1.459188402378541E+08, .z = 2.131411070142508E+06};
  STOP_ON_ERROR("sunFileReadDataPoint index=98", sunFileReadDataPoint(98, &readData));
  sciPrintf("Expected: ", closePositionData(expected3, readData));

  // Get number of data points after JD

  uint32_t count;
  STOP_ON_ERROR("sunFileGetNumDataPointsAfterJD JD=1", sunFileGetNumDataPointsAfterJD(1, &count));
  sciPrintf("Expected count for JD=1: 98\r\n");
  sciPrintf("Count for JD=1: %d\r\n", count);

  STOP_ON_ERROR("sunFileGetNumDataPointsAfterJD JD=33", sunFileGetNumDataPointsAfterJD(33, &count));
  sciPrintf("Expected count for JD=33: 66\r\n");
  sciPrintf("Count for JD=33: %d\r\n", count);

  STOP_ON_ERROR("sunFileGetNumDataPointsAfterJD JD=98", sunFileGetNumDataPointsAfterJD(98, &count));
  sciPrintf("Expected count for JD=98: 1\r\n");
  sciPrintf("Count for JD=98: %d\r\n", count);

  STOP_ON_ERROR("sunFileGetNumDataPointsAfterJD JD=2.5", sunFileGetNumDataPointsAfterJD(2.5, &count));
  sciPrintf("Expected count for JD=2.5: 97\r\n");
  sciPrintf("Count for JD=2.5: %d\r\n", count);
}

int main() {
  sciInit();
  spiInit();

  initSciPrint();
  initSpiMutex();

  sciPrintf("Starting Sun File Tests\r\n");

  xTaskCreateStatic(vTask1, "SunFileTests", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
