/**
 * @file param_manager.c
 * @author Daniel Gobalakrishnan
 * @date 2022-06-01
 */

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"

#include "param_manager.h"
#include "sys_common.h"

// Maybe the param table shouldn't be static
static param_t *param_list[NUM_PARAMS] = &PARAM_TABLE;
static SemaphoreHandle_t param_mutex_arr[NUM_PARAMS];

uint8_t initialize_mutex_array(void)
{
    // TODO: Add any error handling if required
    for (uint8_t i = 0; i < NUM_PARAMS; i++)
    {
        param_mutex_arr[i] = xSemaphoreCreateMutex();
    }
    return 1;
}

param_handle_t get_param_handle(param_names_t param_name)
{
    return &param_list[param_name];
}

uint8_t access_param_table(access_type_t access_type, param_names_t param_name, param_type_t param_type, void *out_p)
{
    param_handle_t param_handle = get_param_handle(param_name);
    if (param_handle == NULL)
        return 0;

    if (param_handle->type != param_type)
        return 0;

    param_size_t param_size = get_param_size(param_type);
    param_val_t param_val = param_handle->val;

    switch (access_type)
    {
    case SET_PARAM:
        memcpy(&param_val, out_p, param_size);
        break;
    case GET_PARAM:
        memcpy(out_p, &param_val, param_size);
        break;
    default:
        return 0;
    }

    return 1;
}

uint8_t get_param_val(param_names_t param_name, param_type_t param_type, void *out_p)
{
    if (param_mutex_arr[param_name] == NULL)
        return 0;

    if (xSemaphoreTake(param_mutex_arr[param_name], portMAX_DELAY) == pdTRUE)
    {
        // TODO: Verify what delay we want to use
        // TODO: Add any error handling if required
        uint8_t status = access_param_table(GET_PARAM, param_name, param_type, out_p);
        xSemaphoreGive(param_mutex_arr[param_name]);
        return status;
    }
    // TODO: Deal with the case where mutex is already taken
}

uint8_t set_param_val(param_names_t param_name, param_type_t param_type, void *in_p)
{
    if (param_mutex_arr[param_name] == NULL)
        return 0;

    if (xSemaphoreTake(param_mutex_arr[param_name], portMAX_DELAY) == pdTRUE)
    {
        // TODO: Verify what delay we want to use
        // TODO: Add any error handling if required
        uint8_t status = access_param_table(SET_PARAM, param_name, param_type, in_p);
        xSemaphoreGive(param_mutex_arr[param_name]);
        return status;
    }
    // Deal with the case where mutex is already taken
}

param_size_t get_param_size(param_type_t type)
{
    switch (type)
    {
    case UINT8_PARAM:
        return UINT8_SIZE;
    case INT8_PARAM:
        return INT8_SIZE;
    case UINT16_PARAM:
        return UINT16_SIZE;
    case INT16_PARAM:
        return INT16_SIZE;
    case UINT32_PARAM:
        return UINT32_SIZE;
    case INT32_PARAM:
        return INT32_SIZE;
    case UINT64_PARAM:
        return UINT64_SIZE;
    case INT64_PARAM:
        return INT64_SIZE;
    case FLOAT_PARAM:
        return FLOAT_SIZE;
    case DOUBLE_PARAM:
        return DOUBLE_SIZE;
    // case STRING_PARAM:
    //     return STRING_SIZE;
    default:
        return 0;
    }
}
