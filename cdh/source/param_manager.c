#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#include "param_manager.h"
#include "sys_common.h"
#include "gio.h"

int get_param_val(param_names_t param, void *out_p)
{
    // Get value from param_list
    return 1;
}

int set_param_val(param_names_t param, void *in_p)
{
    // Set value in param_list
    return 1;
}