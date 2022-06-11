/**
 * @file param_manager.c
 * @author Daniel Gobalakrishnan
 * @date 2022-06-08
 */
#include "param_manager.h"
#include "param_table.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"

#include "string.h"

static param_handle_t paramTableHandler = paramTable;
static SemaphoreHandle_t paramTableMutex = NULL;
static const uint16_t paramTableLength = sizeof(paramTable) / sizeof(param_t);

uint8_t get_param_val(param_names_t paramName, param_type_t paramType, void *out)
{
    uint8_t status = access_param_table(GET_PARAM, paramName, paramType, out);
    xSemaphoreGive(paramTableMutex);
    return status;
}

uint8_t set_param_val(param_names_t paramName, param_type_t paramType, void *in)
{
    uint8_t status = access_param_table(SET_PARAM, paramName, paramType, in);
    xSemaphoreGive(paramTableMutex);
    return status;
}

static uint8_t access_param_table(access_type_t accessType, param_names_t paramName, param_type_t paramType, void *p)
{
    param_handle_t paramHandle = get_param_handle(paramName);
    if ( paramHandle == NULL )
        return 0;

    param_size_t paramSize = get_param_size(paramType);
    if ( paramHandle->type != paramType || paramSize == 0 )
        return 0;

    if ( paramTableMutex == NULL )
        paramTableMutex = xSemaphoreCreateMutex();

    /* TODO: We may want to change the delay time instead of using the max delay */
    if ( xSemaphoreTake(paramTableMutex, portMAX_DELAY) == pdTRUE ) {
        param_val_t paramVal = paramHandle->value;

        switch (accessType) {
            case SET_PARAM:
                memcpy(&(paramHandle->value), p, paramSize);
                break;
            case GET_PARAM:
                memcpy(p, &paramVal, paramSize);
                break;
            default:
                return 0;
        }
    }

    return 1;
}

static param_handle_t get_param_handle(param_names_t paramName)
{
    if ( paramName < 0 || paramName >= paramTableLength )
        return NULL;

    return &paramTableHandler[paramName];
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
    /* case STRING_PARAM: */
    /*     return STRING_SIZE; */
    default:
        return 0;
    }
}

static uint8_t is_read_only(param_handle_t paramHandle) {
    return ( paramHandle->opts & READ_ONLY );
}
static uint8_t is_telemetry(param_handle_t paramHandle) {
    return ( paramHandle->opts & TELEMETRY );
}
static uint8_t is_persistent(param_handle_t paramHandle) {
    return ( paramHandle->opts & PERSISTENT );
}
