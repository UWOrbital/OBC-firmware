#ifndef DRIVERS_INCLUDE_NIS6151
#define DRIVERS_INCLUDE_NIS6151

#include <stdio.h>
#include <gio.h>

/* NIS6151 GIO pin configuration */


/**
 * @struct Configuration struct for NIS6151 eFuse
 * 
 * @param enAddr GIO address of the EN for the NIS6151
 * @param flagAddr GIO address of the FLAG for the NIS6151
 * @param devOperationMode Device operation mode, 0 = normal, 1 = thermal shutdown
*/
typedef struct {
    uint8_t enAddr;
    uint8_t flagAddr;
    uint8_t devOperationMode;
} nis6151_config_t;

/**
 * @brief Initialize the LM75BD
 *
 * @param config Configuration struct for LM75BD
 * @return 1 if successful, 0 otherwise
 */
uint8_t nis6151Init(nis6151_config_t *config);

// TODO: write ISR callback function to deal with FLAG events

#endif /* DRIVERS_INCLUDE_NIS6151 */