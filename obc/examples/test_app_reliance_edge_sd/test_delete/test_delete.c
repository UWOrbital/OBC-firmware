#include "telemetry_fs_utils.h"

#include "obc_reliance_fs.h"
#include "obc_spi_io.h"
#include "obc_sci_io.h"
#include "obc_print.h"
#include "obc_errors.h"
#include "obc_gs_telemetry_data.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

#include <redposix.h>
#include <string.h>

void vTask1(void *pvParameter) {
  obc_error_code_t errCode;

  // Example Telem IDs:
  uint32_t telemBatchId = 0;
  uint32_t telemFileId = 0;
  uint32_t *p_telemFileId = &telemFileId;

  // Example Telem Data:
  telemetry_data_t telemData = {0};
  telemData.obcTemp = 100;
  telemData.id = 67;
  telemData.timestamp = 1000;

  // Init File System:
  RETURN_IF_ERROR_CODE(setupFileSystem());
  sciPrint("Initialized File System");

  // Creating, Writing, and Reading from example file:
  RETURN_IF_ERROR_CODE(createAndOpenTelemetryFileRW(telemBatchId, p_telemFileId));
  sciPrint("Successfully created and opened telemetry file!");

  RETURN_IF_ERROR_CODE(writeTelemetryToFile((int32_t)telemFileId, telemData));
}

int main(void) {}
