#ifndef CDH_INCLUDE_HEALTH_MGR_H_
#define CDH_INCLUDE_HEALTH_MGR_H_

#include <sys_common.h>

#define HEALTH_MGR_POLL_TIME 10     //placeholder


#define HEALTH_STATUS_NORMAL 0
#define HEALTH_STATUS_BAD1 1
#define HEALTH_STATUS_BAD2 2


void initHealthMgr(void);
void heathMgrTakeAction(void);



#endif // CDH_INCLUDE_HEALTH_MGR_H_