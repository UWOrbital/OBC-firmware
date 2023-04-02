#ifndef COMMON_INCLUDE_OBC_RW_LOCK_H_
#define COMMON_INCLUDE_OBC_RW_LOCK_H_

#include <stdint.h>

typedef struct {
    volatile uint32_t seq;
    volatile uint32_t data;
} obc_rw_lock_t;

void rwLockInit(obc_rw_lock_t *lock, uint32_t data);

void rwLockWrite(obc_rw_lock_t *lock, uint32_t data);

uint32_t rwLockRead(obc_rw_lock_t *lock);

#endif /* COMMON_INCLUDE_OBC_RW_LOCK_H_ */