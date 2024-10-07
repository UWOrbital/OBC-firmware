#ifndef CONFIG_SDCARD
#include "obc_errors.h"
#include "obc_general_util.h"
#include "sdc_diskio.h"
#include "sdc_rm46.h"

DSTATUS disk_initialize(uint8_t pdrv) {
  UNUSED(pdrv);
  return 0;
}

DSTATUS disk_status(uint8_t pdrv) {
  UNUSED(pdrv);
  return 0;
}

DRESULT disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint32_t count) {
  UNUSED(pdrv);
  UNUSED(buff);
  UNUSED(sector);
  UNUSED(count);
  return 0;
}

DRESULT disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint32_t count) {
  UNUSED(pdrv);
  UNUSED(buff);
  UNUSED(sector);
  UNUSED(count);
  return 0;
}

DRESULT disk_ioctl(uint8_t pdrv, uint8_t cmd, void* buff) {
  UNUSED(pdrv);
  UNUSED(cmd);
  return 0;
}

#endif  // CONFIG_SDCARD
