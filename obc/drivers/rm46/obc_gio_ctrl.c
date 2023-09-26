#include "gio.h"
#include "hal_stdtypes.h"

#include "cc1120_txrx.h"
#include "alarm_handler.h"
#include "obc_board_config.h"

void gioNotification(gioPORT_t *port, uint32 bit) {
  if (port == gioPORTA) {
    switch (bit) {
      case DS3232_INT_PIN:
        alarmInterruptCallback();
        break;
    }
  } else if (port == gioPORTB) {
    switch (bit) {
      // See section 3.4.1.1
      // Triggered on rising edge so RX FIFO can be read once the signal that RX FIFO is above TXRX_INTERRUPT_THRESHOLD
      // is asserted to avoid a FIFO overflow
      case CC1120_RX_THR_PKT_gioPORTB_PIN:
        rxFifoReadyCallback();
        break;
    }
  }

  if (sci == sciREG) {
    switch (flag) {}
  } else if (sci == scilinREG) {
    transferCompleteSemaphore = sciLinTransferComplete;
  }
