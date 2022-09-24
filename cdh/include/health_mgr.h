#ifndef CDH_INCLUDE_HEALTH_MGR_H_
#define CDH_INCLUDE_HEALTH_MGR_H_


#define HEALTH_MGR_POLL_TIME 10     //placeholder

typedef struct {

} health_report_t;

int healthMgrInit(void);
int healthMgrGenerateReport(health_report_t* report);
int healthMgrParseReport(health_report_t* report);




#endif // CDH_INCLUDE_HEALTH_MGR_H_