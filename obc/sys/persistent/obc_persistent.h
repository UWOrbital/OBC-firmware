#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "obc_errors.h"

// TODO: Change instructions
/*---------------------------------------------------------------------------*/
/* GUIDE FOR ADDING A NEW PERSISTENT SECTION:
 * 1. Add a struct for the data to be stored in the section
 *      - This struct should contain all the data that needs to be stored
 *      - The entire section is protected with a single CRC so the entire
 *        section must be read/written even if you only care about 1 field.
 *        Ex: If you store an array in the section, you must read/write the
 *        entire array even if you only want to change 1 element.
 *      - The struct name should be <module>_persist_data_t
 * 2. Add a struct for the section itself (header + data)
 *      - The struct name should be <module>_persist_t
 *      - This is the layout of the section in persistent storage
 * 3. Add the section struct (header + data) to the obc_persist_t struct
 *      - The struct element name should be <module> (e.g. obcTime)
 *      - obc_persist_t is the layout of the entire persistent storage
 *      - You can declare a buffer of identical sections in this struct.
 *        This is useful for storing arrays that need to be individually
 *        accessible.
 * 4. Create getter/setter for the section
 *     - The getter/setter should be named getPersistent<Module> and
 *       setPersistent<Module> respectively
 *---------------------------------------------------------------------------*/

/**
 * @brief Header struct to be placed at the start of each persistent section
 *
 */
typedef struct {
  size_t len;
  uint32_t crc32;
} obc_persist_section_header_t;

/*---------------------------------------------------------------------------*/

/* SECTION AND DATA STRUCTS */

// obc_time module
typedef struct {
  uint32_t unixTime;
} obc_time_persist_data_t;

typedef struct {
  obc_persist_section_header_t header;
  obc_time_persist_data_t data;
} obc_time_persist_t;

/*---------------------------------------------------------------------------*/
/* CONFIG */

typedef enum {
  OBC_PERSIST_SECTION_ID_OBC_TIME = 0,
  OBC_PERSIST_SECTION_ID_ALARM_MGR,

  OBC_PERSIST_SECTION_ID_COUNT  // Must always be last
} obc_persist_section_id_t;

//? Might want to change the config related to include/excluding headers in the section size/addr
typedef struct {
  uint32_t sectionStartAddr;  // Includes the header
  size_t sectionSize;         // Includes the header
  size_t sectionCount;        // Put as 1 if only 1 section of this type is needed
} obc_persist_config_t;

/* Config */
const obc_persist_config_t obcPersistConfig[] = {
    [OBC_PERSIST_SECTION_ID_OBC_TIME] = {OBC_PERSIST_ADDR_OF(obcTime), sizeof(obc_time_persist_t), 1},
    //  [OBC_PERSIST_SECTION_ID_ALARM_MGR] = {OBC_PERSIST_ADDR_OF(alarmMgr), sizeof(obc_alarm_mgr_persist_t), 1}
};

/*---------------------------------------------------------------------------*/

/**
 * @brief Layout of the persistent storage
 */
typedef struct {
  obc_time_persist_t obcTime;
} obc_persist_t;

#define OBC_PERSIST_ADDR_OF(data) (0x0 + offsetof(obc_persist_t, data))

/*---------------------------------------------------------------------------*/

/* GETTERS AND SETTERS */

/**
 * @brief Get a persistent section from FRAM by the sectionId and verify its header data
 *
 * @param sectionId The sectionId of the section to get
 * @param buff Buffer to store the section (header not included)
 * @param buffLen Length of the buffer (Must be at least sectionSize - sizeof(obc_persist_section_header_t) bytes long)
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t getPersistentSection(obc_persist_section_id_t sectionId, uint8_t *buff, size_t buffLen);

/**
 * @brief Set a persistent section in FRAM by the sectionId and write its header data
 *
 * @param sectionId The sectionId of the section to set
 * @param buff Buffer containing the section data (header not included)
 * @param buffLen Length of the buffer (Must be at least sectionSize - sizeof(obc_persist_section_header_t) bytes long)
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t setPersistentSection(obc_persist_section_id_t sectionId, const uint8_t *buff, size_t buffLen);

/**
 * @brief Get a persistent section from FRAM by the sectionId and subIndex and verify its header data. This function is
 * used when the section is an array of identical sections.
 *
 * @param sectionId The sectionId of the section to get
 * @param subIndex The subIndex of the section to get
 * @param buff Buffer to store the section data (header not included)
 * @param buffLen Length of the buffer (Must be at least sectionSize - sizeof(obc_persist_section_header_t) bytes long)
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t getPersistentSectionBySubIndex(obc_persist_section_id_t sectionId, size_t subIndex, uint8_t *buff,
                                                size_t buffLen);

/**
 * @brief Set a persistent section in FRAM by the sectionId and subIndex and write its header data. This function is
 * used when the section is an array of identical sections.
 *
 * @param sectionId The sectionId of the section to set
 * @param subIndex The subIndex of the section to get
 * @param buff Buffer containing the section data (header not included)
 * @param buffLen Length of the buffer (Must be at least sectionSize - sizeof(obc_persist_section_header_t) bytes long)
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t setPersistentSectionBySubIndex(obc_persist_section_id_t sectionId, size_t subIndex,
                                                const uint8_t *buff, size_t buffLen);

// obc_error_code_t getPersistentObcTime(obc_time_persist_data_t *buffer);
// obc_error_code_t setPersistentObcTime(obc_time_persist_data_t *data);
