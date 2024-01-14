#pragma once

#include "obc_errors.h"

#include <sys_common.h>

/**
 * @enum	gnc_event_id_t
 * @brief	gnc event ID enum.
 *
 * Enum containing all possible event IDs passed to the gnc event queue.
 */
typedef enum {
  GNC_MANAGER_NULL_EVENT_ID,
  SECONDARY_GNC_MANAGER_EVENT_ID,
} gnc_event_id_t;

/**
 * @union	gnc_event_data_t
 * @brief	gnc event data union
 */
typedef union {
  int i;
  float f;
} gnc_event_data_t;

/**
 * @struct	gnc_event_t
 * @brief	gnc event struct
 *
 * Holds the message data for each event sent/received by the gnc manager queue.
 */
typedef struct {
  gnc_event_id_t eventID;
  gnc_event_data_t data;
} gnc_event_t;

/* gnc queue config */
#define GNC_MANAGER_QUEUE_LENGTH 10U
#define GNC_MANAGER_QUEUE_ITEM_SIZE sizeof(gnc_event_t)
#define GNC_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define GNC_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Send an event to the gnc Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToGncQueue(gnc_event_t *event);
