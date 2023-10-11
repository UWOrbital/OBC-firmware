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

obc_error_code_t setPersistentSectionBySubIndex(obc_persist_section_id_t sectionId, size_t subIndex,
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

/*---------------------------------------------------------------------*/
/*------------------------- Private functions -------------------------*/
/*---------------------------------------------------------------------*/

static obc_persist_config_t *getOBCPersistConfig(obc_persist_section_id_t sectionId) {
  if (sectionId >= OBC_PERSIST_SECTION_ID_COUNT) return NULL;

  return &obcPersistConfig[sectionId];
}
