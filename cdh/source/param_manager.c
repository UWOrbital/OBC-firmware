#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#include "param_manager.h"
#include "sys_common.h"
#include "gio.h"

param_t *param_list[NUM_PARAMS] = &PARAM_TABLE;

param_handle_t get_param(param_names_t param_name)
{
    return &param_list[param_name];
}

int get_param_val(param_names_t param_name, param_type_t param_type, void *out_p)
{
    param_handle_t param_handle = get_param(param_name);
    if (param_handle == NULL)
        return 0;

    if (param_handle->type != param_type)
        return 0;

    uint16_t param_size = get_param_size(param_type);
    memcpy(out_p, &(param_handle->value), param_size);
    return 1;
}

int set_param_val(param_names_t param_name, param_type_t param_type, void *in_p)
{
    param_handle_t param_handle = get_param(param_name);
    if (param_handle == NULL)
        return 0;

    if (param_handle->type != param_type)
        return 0;

    uint16_t param_size = get_param_size(param_type);
    memcpy(&(param_handle->value), in_p, param_size);
    return 1;
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
    case STRING_PARAM:
        // TODO: return the size of the string after deciding the size
        // return STRING_SIZE;
        printf("String size not yet decided\n");
        break;
    default:
        return 0;
    }
}

int main()
{
    // FOR TESTING PURPOSES
    return 0;
}