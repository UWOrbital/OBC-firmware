/**
 * @file param_manager.c
 * @author Daniel Gobalakrishnan
 * @date 2022-06-01
 */

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"

#include "param_manager.h"
#include "param_table.h"
#include "string.h"

static param_handle_t param_list = PARAM_TABLE;
// TODO: Research whether using a single mutex to protect the entire table would be more efficient...
// Need to do more research into it:
// https://stackoverflow.com/questions/24377671/efficiency-of-array-with-individual-mutexes-protecting-them-or-one-mutex-protect
static SemaphoreHandle_t param_mutex_arr[NUM_PARAMS];

// TODO: Deal with parameter options

uint8_t initialize_mutex_array(void)
{
    // TODO: Add any error handling if required
    for (uint8_t i = 0; i < NUM_PARAMS; i++)
    {
        param_mutex_arr[i] = xSemaphoreCreateMutex();
    }
    return 1;
}

uint8_t get_param_val(param_names_t param_name, param_type_t param_type, void *out_p)
{
    if (param_name < 0 || param_name >= NUM_PARAMS)
        return 0;

    if (param_mutex_arr[param_name] == NULL)
        return 0;

    /* TODO: We may want to change the delay time instead of using the max delay */
    if (xSemaphoreTake(param_mutex_arr[param_name], portMAX_DELAY) == pdTRUE)
    {
        uint8_t status = access_param_table(GET_PARAM, param_name, param_type, out_p);
        xSemaphoreGive(param_mutex_arr[param_name]);
        return status;
    }
    return 0;
}

uint8_t set_param_val(param_names_t param_name, param_type_t param_type, void *in_p)
{
    if (param_name < 0 || param_name >= NUM_PARAMS)
        return 0;

    if (param_mutex_arr[param_name] == NULL)
        return 0;

    /* TODO: We may want to change the delay time instead of using the max delay */
    if (xSemaphoreTake(param_mutex_arr[param_name], portMAX_DELAY) == pdTRUE) {
        uint8_t status = access_param_table(SET_PARAM, param_name, param_type, in_p);
        xSemaphoreGive(param_mutex_arr[param_name]);
        return status;
    }
    return 0;
}

static param_handle_t get_param_handle(param_names_t param_name)
{
    return &param_list[param_name];
}

static uint8_t access_param_table(access_type_t access_type, param_names_t param_name, param_type_t param_type, void *out_p)
{
    // Maybe we should add a mutex around the param_handle access
    param_handle_t param_handle = get_param_handle(param_name);
    if (param_handle == NULL)
        return 0;

    if (param_handle->type != param_type)
        return 0;

    param_size_t param_size = get_param_size(param_type);
    param_val_t param_val = param_handle->value;

    switch (access_type)
    {
    case SET_PARAM:
        memcpy(&(param_handle->value), out_p, param_size);
        break;
    case GET_PARAM:
        memcpy(out_p, &param_val, param_size);
        break;
    default:
        return 0;
    }

    return 1;
}

static param_size_t get_param_size(param_type_t type)
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
