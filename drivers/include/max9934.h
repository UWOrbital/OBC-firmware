#ifndef DRIVERS_INCLUDE_MAX9934_H_
#define DRIVERS_INCLUDE_MAX9934_H_

#include <stdint.h>

typedef struct {
    uint8_t devAddr;
} max9934_config_t;


/**
 * @brief Initialize the MAX9934
 * 
 * @param config Configuration struct for MAX9934
 * @return 1 if succesful, 0 otherwise
 */

uint8_t max9934init(max9934_config_t *config);

/**
 * @brief Read the current from the max9334
 */



#endif /* DRIVERS_INCLUDE_MAX9934_H_ */