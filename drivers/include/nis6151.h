#ifndef DRIVERS_INCLUDE_NIS6151
#define DRIVERS_INCLUDE_NIS6151

#include <gio.h>

/* NIS6151 GIO pin configuration */
#define NIS6151_GIOPORT gioPORTB
#define NIS6151_EN_BIT 0U
#define NIS6151_FLAG_BIT 1U /* Unused for now */

/**
 * @brief Enables the NIS6151
 * @note EN pin has internal pull -> GIOB pin 0 set as open-drain
 * @return 1 if successful, 0 otherwise
 */
uint8_t enableNIS6151(void);

/**
 * @brief Disables the NIS6151
 * @note EN pin has internal pull -> GIOB pin 0 set as open-drain
 * @return 1 if successful, 0 otherwise
 */
uint8_t disableNIS6151(void);

#endif /* DRIVERS_INCLUDE_NIS6151 */