#include "het.h"
#include "hal_stdtypes.h"

#include "cc1120_txrx.h"
#include "obc_board_config.h"

#include <stdint.h>

void edgeNotification(hetBASE_t* hetREG, uint32 edge) {
  if (hetREG == hetREG1) {
    switch (edge) {
      // See section 3.4.1.1
      // Triggered on falling edge so TX FIFO can be written to once the signal that RTX FIFO is above
      // TXRX_INTERRUPT_THRESHOLD is deasserted to avoid a FIFO
      case CC1120_TX_THR_PKT_hetPORT1_PIN:
        txFifoReadyCallback();
        break;
      // See section 3.4.1.1
      // triggered on falling edge once TX FIFO has been completely emptied
      case CC1120_PKT_SYNC_RXTX_hetPORT1_PIN:
        txFifoEmptyCallback();
        break;
    }
  }
}
