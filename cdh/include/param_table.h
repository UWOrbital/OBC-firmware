/**
 * @file param_manager.c
 * @author Daniel Gobalakrishnan
 * @date 2022-06-08
 */
#ifndef PARAM_TABLE_H
#define PARAM_TABLE_H

#include "param_manager.h"

/**
 * @brief Parameter table
 *
 * All parameters and their associated properties
 * @warning    Remember to add any new parameters to the param_names_t enum and increment
 * the NUM_PARAMS macro
 * @see param_names_t
*/
param_t paramTable [NUM_PARAMS] = {
        {.name=ALTITUDE, .type=INT8_PARAM, .size=DOUBLE_SIZE, .opts=TELEMETRY, .value.i8=30},
        {.name=TEST_PARAM1, .type=UINT8_PARAM, .size=UINT8_SIZE, .opts=PERSISTENT|TELEMETRY, .value.i8=0},
        {.name=TEST_PARAM2, .type=UINT16_PARAM, .size=UINT16_SIZE, .opts=PERSISTENT, .value.i16=0},
        {.name=TEST_PARAM3, .type=UINT32_PARAM, .size=UINT32_SIZE, .opts=PERSISTENT|READ_ONLY, .value.i32=0},
        {.name=TEST_PARAM4, .type=UINT64_PARAM, .size=UINT64_SIZE, .opts=PERSISTENT, .value.i64=0},
        {.name=TEST_PARAM5, .type=INT8_PARAM, .size=INT8_SIZE, .opts=PERSISTENT, .value.i8=0},
        {.name=TEST_PARAM6, .type=INT16_PARAM, .size=INT16_SIZE, .opts=PERSISTENT, .value.i16=0},
        {.name=TEST_PARAM7, .type=INT32_PARAM, .size=INT32_SIZE, .opts=PERSISTENT|READ_ONLY, .value.i32=0},
        {.name=TEST_PARAM8, .type=INT64_PARAM, .size=INT64_SIZE, .opts=PERSISTENT, .value.i64=0},
        {.name=TEST_PARAM9, .type=FLOAT_PARAM, .size=FLOAT_SIZE, .opts=PERSISTENT, .value.f=0},
        {.name=TEST_PARAM10, .type=DOUBLE_PARAM, .size=DOUBLE_SIZE, .opts=PERSISTENT, .value.d=0}
};

#endif /* PARAM_TABLE_H */