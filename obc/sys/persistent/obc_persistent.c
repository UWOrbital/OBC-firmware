#include "obc_persistent.h"

#include "fm25v20a.h"
#include "obc_assert.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_crc.h"
#include "obc_assert.h"

#include <string.h>

STATIC_ASSERT(sizeof(obc_persist_t) <= FRAM_MAX_ADDRESS, "obc_persist_t exceeds available FRAM space");

/**
 * @brief Get a persistent section from FRAM and verify its header data
 *
 * @param sectionStartAddr Start address of the section in FRAM
 * @param sectionSize Size of the section in FRAM
 * @param buff Buffer to store the section
 * @param buffLen Length of the buffer (Must be at least sectionSize bytes long)
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t getPersistentSection(uint32_t sectionStartAddr, size_t sectionSize, uint8_t *buff,
                                             size_t buffLen);

/**
 * @brief Set a persistent section in FRAM and write its header data
 *
 * @param sectionStartAddr Start address of the section in FRAM
 * @param sectionSize Size of the section in FRAM
 * @param buff Buffer containing the section data (Layout of buffer must match the section struct; leave space for
 * header data)
 * @param buffLen Length of the buffer (Must be at least sectionSize bytes long)
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t setPersistentSection(uint32_t sectionStartAddr, size_t sectionSize, uint8_t *buff,
                                             size_t buffLen);

/**
 * @brief Overlay data in a section buffer. Data is placed after the header part of the section buffer.
 *
 * @param data The data to overlay
 * @param dataLen Length of the data
 * @param sectionBuff The section buffer to overlay the data in
 * @param sectionLen Length of the section buffer
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t overlayDataInSectionBuff(uint8_t *data, size_t dataLen, uint8_t *sectionBuff,
                                                 size_t sectionLen);

/**
 * @brief Extract data from a section buffer. Data is extracted after the header part of the section buffer.
 *
 * @param data Buffer to store the extracted data in
 * @param dataLen Length of the data buffer
 * @param sectionBuff The section buffer to extract the data from
 * @param sectionLen Length of the section buffer
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t extractDataFromSectionBuff(uint8_t *data, size_t dataLen, uint8_t *sectionBuff,
                                                   size_t sectionLen);

obc_error_code_t getPersistentObcTime(obc_time_persist_data_t *data) {
  obc_error_code_t errCode;

  uint8_t sectionBuffer[sizeof(obc_time_persist_t)] = {0};

  // Read entire section (including header)
  RETURN_IF_ERROR_CODE(
      getPersistentSection(OBC_PERSIST_ADDR_OF(obcTime), sizeof(sectionBuffer), sectionBuffer, sizeof(sectionBuffer)));

  // Strip header from section buffer
  RETURN_IF_ERROR_CODE(
      extractDataFromSectionBuff((uint8_t *)data, sizeof(*data), sectionBuffer, sizeof(sectionBuffer)));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentObcTime(obc_time_persist_data_t *data) {
  obc_error_code_t errCode;

  uint8_t sectionBuffer[sizeof(obc_time_persist_t)] = {0};

  // Place in buffer with space for header
  RETURN_IF_ERROR_CODE(overlayDataInSectionBuff((uint8_t *)data, sizeof(*data), sectionBuffer, sizeof(sectionBuffer)));

  // Write entire section (including header)
  RETURN_IF_ERROR_CODE(
      setPersistentSection(OBC_PERSIST_ADDR_OF(obcTime), sizeof(sectionBuffer), sectionBuffer, sizeof(sectionBuffer)));

  return OBC_ERR_CODE_SUCCESS;
}


obc_error_code_t getPersistentAlarmMgr(alarm_mgr_persist_data_t *data) {
  obc_error_code_t errCode;

  uint8_t sectionBuffer[sizeof(alarm_mgr_persist_t)] = {0};
  RETURN_IF_ERROR_CODE(
      getPersistentSection(OBC_PERSIST_ADDR_OF(alarmMgr), sizeof(sectionBuffer), sectionBuffer, sizeof(sectionBuffer)));

  RETURN_IF_ERROR_CODE(
      extractDataFromSectionBuff((uint8_t *)data, sizeof(*data), sectionBuffer, sizeof(sectionBuffer)));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentAlarmMgr(alarm_mgr_persist_data_t *data) {
  obc_error_code_t errCode;

  uint8_t sectionBuffer[sizeof(alarm_mgr_persist_data_t)] = {0};

  RETURN_IF_ERROR_CODE(overlayDataInSectionBuff((uint8_t *)data, sizeof(*data), sectionBuffer, sizeof(sectionBuffer)));

  RETURN_IF_ERROR_CODE(
      setPersistentSection(OBC_PERSIST_ADDR_OF(alarmMgr), sizeof(sectionBuffer), sectionBuffer, sizeof(sectionBuffer)));

  return OBC_ERR_CODE_SUCCESS;
}


/*---------------------------------------------------------------------*/
/*------------------------- Private functions -------------------------*/
/*---------------------------------------------------------------------*/

static obc_error_code_t getPersistentSection(uint32_t sectionStartAddr, size_t sectionSize, uint8_t *buff,
                                             size_t buffLen) {
  obc_error_code_t errCode;

  if (buff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (buffLen < sectionSize) {
    return OBC_ERR_CODE_BUFF_TOO_SMALL;
  }

  if (sectionSize < sizeof(obc_persist_section_header_t)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(framRead(sectionStartAddr, buff, sectionSize));

  obc_persist_section_header_t header = {0};
  memcpy(&header, buff, sizeof(obc_persist_section_header_t));

  if (header.len != sectionSize) {
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  uint8_t *data = buff + sizeof(obc_persist_section_header_t);
  uint32_t crc32 = computeCrc32(0, data, sectionSize - sizeof(obc_persist_section_header_t));
  if (header.crc32 != crc32) {
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t setPersistentSection(uint32_t sectionStartAddr, size_t sectionSize, uint8_t *buff,
                                             size_t buffLen) {
  obc_error_code_t errCode;

  if (buff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (sectionSize < sizeof(obc_persist_section_header_t)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (buffLen < sectionSize) {
    return OBC_ERR_CODE_BUFF_TOO_SMALL;
  }

  // Assume buff is already formatted with space for header at the start
  obc_persist_section_header_t header = {0};
  header.len = sectionSize;
  header.crc32 =
      computeCrc32(0, buff + sizeof(obc_persist_section_header_t), sectionSize - sizeof(obc_persist_section_header_t));

  memcpy(buff, &header, sizeof(obc_persist_section_header_t));

  RETURN_IF_ERROR_CODE(framWrite(sectionStartAddr, buff, sectionSize));

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t overlayDataInSectionBuff(uint8_t *data, size_t dataLen, uint8_t *sectionBuff,
                                                 size_t sectionLen) {
  if (data == NULL || sectionBuff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (dataLen > sectionLen - sizeof(obc_persist_section_header_t)) {
    return OBC_ERR_CODE_BUFF_TOO_SMALL;
  }

  memcpy(sectionBuff + sizeof(obc_persist_section_header_t), data, dataLen);
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t extractDataFromSectionBuff(uint8_t *data, size_t dataLen, uint8_t *sectionBuff,
                                                   size_t sectionLen) {
  if (data == NULL || sectionBuff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (dataLen > sectionLen - sizeof(obc_persist_section_header_t)) {
    return OBC_ERR_CODE_BUFF_TOO_SMALL;
  }

  memcpy(data, sectionBuff + sizeof(obc_persist_section_header_t), dataLen);
  return OBC_ERR_CODE_SUCCESS;
}
