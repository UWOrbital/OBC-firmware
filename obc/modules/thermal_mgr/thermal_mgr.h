#pragma once

#include "obc_errors.h"
#include <sys_common.h>

/**
 * @enum	thermal_event_id_t
 * @brief	thermal event ID enum.
 *
 * Enum containing all possible event IDs passed to the thermal manager event queue.
 */
typedef enum {
  THERMAL_MANAGER_NULL_EVENT_ID,
  TELEMETRY_THERMAL_MANAGER_EVENT_ID,
  BATTERY_THERMAL_MANAGER_EVENT_ID,
  OVERTEMPERATURE_THERMAL_MANAGER_EVENT_ID
} thermal_mgr_event_id_t;

/**
 * @union	thermal_mgr_event_data_t
 * @brief	thermal maanger event data union
 */
typedef union {
  int i;
  float f;
} thermal_mgr_event_data_t;

/**
 * @struct	thermal_mgr_event_t
 * @brief	thermal manager event struct
 *
 * Holds the message data for each event sent/received by the thermal manager queue.
 */
typedef struct {
  thermal_mgr_event_id_t eventID;
  thermal_mgr_event_data_t data;
} thermal_mgr_event_t;

/* payload queue config */
#define THERMAL_MANAGER_QUEUE_LENGTH 10U
#define THERMAL_MANAGER_QUEUE_ITEM_SIZE sizeof(thermal_mgr_event_t)
#define THERMAL_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define THERMAL_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Send an event to the Thermal Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */

obc_error_code_t sendToThermalMgrQueue(thermal_mgr_event_t *event);
