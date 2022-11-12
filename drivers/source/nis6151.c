#include "nis6151.h"
#include "obc_sci_io.h"

void enableNIS6151(void) {
    /* OBC pin to NIS6151 EN is configured as open-drain -> 0 to set floating */
    gioSetBit(NIS6151_GIOPORT, NIS6151_EN_BIT, 0);
}

void disableNIS6151(void) {
    /* OBC pin to NIS6151 EN is configured as open-drain -> 1 to pull to ground */
    gioSetBit(NIS6151_GIOPORT, NIS6151_EN_BIT, 1);
}
