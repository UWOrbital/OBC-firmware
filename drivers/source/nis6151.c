#include "nis6151.h"

uint8_t nis6151Init(nis6151_config_t *config) {
    if(config == NULL) {
        return 0;
    }

    /* configure GIOB0 as output and GIOB1 as interupt */
    gioSetBit(gioPORTB, 0, 1);
    gioEnableNotification(gioPORTB, 1);

    return 1;
}