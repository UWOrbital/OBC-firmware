#pragma once

#include <stdint.h>

// Must be less than 256 states
#define OBC_STATE_INITIALIZING (uint8_t)0
#define OBC_STATE_NORMAL (uint8_t)1
#define OBC_STATE_LOW_POWER (uint8_t)2
