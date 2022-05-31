#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#include "param_manager.h"
#include "sys_common.h"
#include "gio.h"

param_t *param_list[NUM_PARAMS] = &PARAM_TABLE;

int get_param_val(param_names_t param, void *out_p, uint8_t size)
{
    param_t *p = param_list[param];
    if (p == NULL)
        return 0;
    if (p->size != size)
        return 0;
    memcpy(out_p, p->value, size);
    return 1;
}

int set_param_val(param_names_t param, void *in_p, uint8_t size)
{
    param_t *p = param_list[param];
    if (p == NULL)
        return 0;
    if (p->size != size)
        return 0;
    memcpy(p->value, in_p, size);
    return 1;
}

int main() 
{
    // FOR TESTING PURPOSES
    return 0;
}