#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "obc_errors.h"
#include "alarm_handler.h"

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

// alarm_mgr module
typedef struct {
  uint32_t unixTime;
  alarm_type_t type;
  alarm_handler_callback_def_t callbackDef;
  union {
    cmd_msg_t cmdMsg;
  };

} alarm_mgr_persist_data_t;

typedef struct {
  obc_persist_section_header_t header;
  alarm_mgr_persist_data_t data;

} alarm_mgr_persist_t;

/*---------------------------------------------------------------------------*/

/**
 * @brief Layout of the persistent storage
 */
typedef struct {
  obc_time_persist_t obcTime;

  alarm_mgr_persist_t alarmMgr;

} obc_persist_t;

#define OBC_PERSIST_ADDR_OF(data) (0x0 + offsetof(obc_persist_t, data))

/*---------------------------------------------------------------------------*/

/* GETTERS AND SETTERS */

obc_error_code_t getPersistentObcTime(obc_time_persist_data_t *buffer);
obc_error_code_t setPersistentObcTime(obc_time_persist_data_t *data);

obc_error_code_t getPersistentAlarmMgr(alarm_mgr_persist_data_t *buffer);
obc_error_code_t setPersistentAlarmMgr(alarm_mgr_persist_data_t *data);
