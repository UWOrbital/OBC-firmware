#ifndef CDH_INCLUDE_HEALTH_MGR_H_
#define CDH_INCLUDE_HEALTH_MGR_H_

#include <sys_common.h>

#define HEALTH_MGR_POLL_TIME 69     //placeholder
#define NUM_HEALTH_PARAMS 2     //placeholder


typedef enum {
    HEALTH_STATUS_NORMAL,
    HEALTH_STATUS_BAD1,
    HEALTH_STATUS_BAD2,
} health_status_t;

// typedef union {
//     int i;
//     float f;
// } health_param_data_t;



typedef enum {
    HEALTH_PARAM_VERIF_EQUALS,
    HEALTH_PARAM_VERIF_GREATER_THAN,
    HEALTH_PARAM_VERIF_LESS_THAN,
} health_param_verif_t;


typedef struct {
    uint8_t id;
    uint32_t value;
    uint32_t expected;
    health_param_verif_t verif;
} health_param_t;


void initHealthMgr(void);
void heathMgrTakeAction(void);
void heathParamSetExpected(health_param_t* param);
void heathParamSetValue(health_param_t* param);
uint8_t heathVerifParam(health_param_t* param);



#endif // CDH_INCLUDE_HEALTH_MGR_H_