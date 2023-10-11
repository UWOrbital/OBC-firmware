#include "obc_persistent.h"

#include "fm25v20a.h"
#include "obc_assert.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_crc.h"
#include "obc_assert.h"

#include <string.h>

STATIC_ASSERT(sizeof(obc_persist_t) <= FRAM_MAX_ADDRESS, "obc_persist_t exceeds available FRAM space");

/* Private function declarations */

/**
 * @brief Gets the config based on the section id
 * @return obc_persist_config_t* Pointer to the config or NULL if sectionId is invalid
 */
static obc_persist_config_t *getOBCPersistConfig(obc_persist_section_id_t sectionId);

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

/* Public function definitions */

obc_error_code_t getPersistentSection(obc_persist_section_id_t sectionId, uint8_t *buff, size_t buffLen) {
  return getPersistentSectionBySubIndex(sectionId, 0, buff, buffLen);
}

obc_error_code_t setPersistentSection(obc_persist_section_id_t sectionId, const uint8_t *buff, size_t buffLen) {
  return setPersistentSectionBySubIndex(sectionId, 0, buff, buffLen);
}

obc_error_code_t getPersistentSectionBySubIndex(obc_persist_section_id_t sectionId, size_t subIndex, uint8_t *buff,
                                                size_t buffLen) {
  obc_error_code_t errCode;
  if (buff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Validate against config
  obc_persist_config_t *config = getOBCPersistConfig(sectionId);
  if (config == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (subIndex >= config->sectionCount) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Buffer will only hold the data not the header
  if (buffLen < config->sectionSize - sizeof(obc_persist_section_header_t)) {
    return OBC_ERR_CODE_BUFF_TOO_SMALL;
  }
  uint32_t sectionStartAddrBySubIndex = config->sectionStartAddr + subIndex * config->sectionSize;

  // Reads header, assumes sectionSize holds the header as well
  uint8_t headerBuffer[sizeof(obc_persist_section_header_t)] = {0};
  RETURN_IF_ERROR_CODE(framRead(sectionStartAddrBySubIndex, headerBuffer, sizeof(obc_persist_section_header_t)));
  obc_persist_section_header_t header = {0};
  memcpy(&header, headerBuffer, sizeof(obc_persist_section_header_t));

  if (header.len != config->sectionSize) {
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  // Compute CRC32 of data and check it
  uint32_t crc32 = computeCrc32(0, buff, buffLen);
  if (header.crc32 != crc32) {
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  RETURN_IF_ERROR_CODE(framRead(sectionStartAddrBySubIndex + sizeof(obc_persist_section_header_t), buff, buffLen));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentSectionBySubIndex(obc_persist_section_id_t sectionId, size_t subIndex,
                                                const uint8_t *buff, size_t buffLen) {
  obc_error_code_t errCode;

  if (buff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Validate against config
  obc_persist_config_t *config = getOBCPersistConfig(sectionId);
  if (config == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (subIndex >= config->sectionCount) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Buffer will only hold the data not the header
  if (buffLen < config->sectionSize - sizeof(obc_persist_section_header_t)) {
    return OBC_ERR_CODE_BUFF_TOO_SMALL;
  }

  // Create header
  obc_persist_section_header_t header = {0};
  header.len = config->sectionSize;
  header.crc32 = computeCrc32(0, buff, buffLen);

  // Write header and data
  uint32_t sectionStartAddrBySubIndex = config->sectionStartAddr + subIndex * config->sectionSize;
  RETURN_IF_ERROR_CODE(framWrite(sectionStartAddrBySubIndex, &header, sizeof(obc_persist_section_header_t)));
  RETURN_IF_ERROR_CODE(framWrite(sectionStartAddrBySubIndex + sizeof(obc_persist_section_header_t), buff, buffLen));

  return OBC_ERR_CODE_SUCCESS;
}

// TODO: Remove these functions

/**
 * @brief Get a persistent section from FRAM and verify its header data
 *
 * @param sectionStartAddr Start address of the section in FRAM
 * @param sectionSize Size of the section in FRAM
 * @param buff Buffer to store the section
 * @param buffLen Length of the buffer (Must be at least sectionSize bytes long)
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t getPersistentSectionOLD(uint32_t sectionStartAddr, size_t sectionSize, uint8_t *buff,
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
static obc_error_code_t setPersistentSectionOLD(uint32_t sectionStartAddr, size_t sectionSize, uint8_t *buff,
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

/*---------------------------------------------------------------------*/
/*------------------------- Private functions -------------------------*/
/*---------------------------------------------------------------------*/

static obc_persist_config_t *getOBCPersistConfig(obc_persist_section_id_t sectionId) {
  if (sectionId >= OBC_PERSIST_SECTION_ID_COUNT) return NULL;

  return &obcPersistConfig[sectionId];
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
