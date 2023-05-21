#ifndef COMMON_INCLUDE_OBC_PERSISTENT_DATA_CONFIG_H_
#define COMMON_INCLUDE_OBC_PERSISTENT_DATA_CONFIG_H_

#include <stdlib.h>
#include <stdint.h>

//System data
typedef struct {
    uint32_t unix_time;
    uint32_t obc_state;
} fram_sys_data_t;

//ADCS data
typedef struct {
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
} fram_adcs_data_t;

//COMMS data
typedef struct {
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
} fram_comms_data_t;

//CDH data
typedef struct {
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
} fram_cdh_data_t;

//EPS data
typedef struct {
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
} fram_eps_data_t;

typedef struct {
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
} fram_payload_data_t;

#endif  //COMMON_INCLUDE_OBC_PERSISTENT_DATA_CONFIG_H_