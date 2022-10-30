#include "nis6151.h"
#include "obc_sci_io.h"

#include <string.h>

uint8_t enableNIS6151(void) {
    /* OBC pin to NIS6151 EN is configured as open-drain -> 0 to set floating */
    gioSetBit(NIS6151_GIOPORT, NIS6151_EN_BIT, 0);

    return 1;
}

uint8_t disableNIS6151(void) {
    /* OBC pin to NIS6151 EN is configured as open-drain -> 1 to pull to ground */
    gioSetBit(NIS6151_GIOPORT, NIS6151_EN_BIT, 1);

    return 1;
}
