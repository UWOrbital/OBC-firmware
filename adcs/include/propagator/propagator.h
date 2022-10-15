#ifndef ADCS_INCLUDE_PROPAGATOR_PROPAGATOR_H_
#define ADCS_INCLUDE_PROPAGATOR_PROPAGATOR_H_

#include <FreeRTOS.h>

#define TLE_PROPAGATOR_MSG_BUFFER_SIZE 256
#define TLE_PROPAGATOR_MSG_BUFFER_WAIT_TICKS 5/portTICK_PERIOD_MS

#define PROPAGATOR_DELAY_TICKS 1000/portTICK_PERIOD_MS

/**
 * @brief TLE data.
 * 
 * This is the data format for a two-line element set (TLE). Note that
 * each line is only 69 characters without a \0 delimiter.
 */
typedef struct {
    char line_1[69];
    char line_2[69];
} tle_msg_t;

/**
 * @brief Orbital propagator from TLE.
 * 
 * @param pvParameters Task parameters.
 */
void vOrbitPropagatorTask(void * pvParameters);

#endif /* ADCS_INCLUDE_PROPAGATOR_PROPAGATOR_H_ */