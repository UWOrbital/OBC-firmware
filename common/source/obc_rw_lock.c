#include "obc_rw_lock.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>

#include <stdint.h>

// This is a read-write lock that allows multiple readers, but only 1 writer.

void rwLockInit(obc_rw_lock_t *lock, uint32_t data) {
    lock->seq = 0;
    lock->data = data;
}

void rwLockWrite(obc_rw_lock_t *lock, uint32_t data) {
    portENTER_CRITICAL();

    lock->data = data;
    lock->seq += 2; // This will wrap around to 0 if it overflows

    portEXIT_CRITICAL();
}

uint32_t rwLockRead(obc_rw_lock_t *lock) {
    uint32_t seq;
    uint32_t data;

    do {
        seq = lock->seq;
        data = lock->data;
    } while (seq != lock->seq);

    return data;
}
