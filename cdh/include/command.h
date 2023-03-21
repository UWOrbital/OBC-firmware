#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

/*command IDs*/
#define NUM_CMDS 14
#define CMD_TLE_DATA_UPLINK (uint8_t) 1 //based on the google sheets, first command starts as ID = 1. Should it be 0 instead?
#define CMD_ARO_DATA_UPLINK (uint8_t) 2
#define CMD_SECONDARY_PAYLOAD_ACTIVATION (uint8_t) 3
#define CMD_RTC_SYNC (uint8_t) 4
#define CMD_SECONDARY_PAYLOAD_ACTIVATION_TIME (uint8_t) 5
#define CMD_CALL_EPS_MCU_RESET (uint8_t) 6
#define CMD_CALL_CDH_MCU_RESET (uint8_t) 7
#define CMD_TIME_DELAY_IMG_CAPTURE_AND_DOWNLINK (uint8_t) 8
#define CMD_DIRECT_EPS_POWER_RAIL_CONTROL (uint8_t) 9
#define CMD_COMMS_ECHO_BACK (uint8_t) 10
#define CMD_CHANGE_VALUE_PARAM_MNGER (uint8_t) 11
#define CMD_ECHO_PARM_MNGER_VALUE (uint8_t) 12
#define CMD_MANUALLY_ACTIVATE_EMERGENCY_MODE_SPECIFIED_TIME (uint8_t) 13
#define CMD_DOWNLINK_FULL_SYS_LOGS_NEXT_PASS (uint8_t) 14

#define COMMAND_QUEUE_LENGTH 10
#define COMMAND_QUEUE_SIZE sizeof(commandTemplate)

//#include<ds3232_mz.c> //file for the RTC alarm functions
//check message buffer instead of making a queue everytime executing command with time tag
//store in FRAM and RTC alarm system. Talk to daniel

/*data structs*/

typedef struct {
    char commandName[30];
    int timeTag;
	float inclination;
	float RAAN;
	float eccentricity;
	float argPerigee;
	float meanAnomaly;
	double meanMotion;
	int numRevolution;
} commandTemplate;

// CMD_TLE_DATA_UPLINK
typedef struct {
	float inclination;
	float raan;
	float eccentricity;
	float argOfPedigree;
	float meanAnomaly;
	double meanMotion;
	uint32_t revolution;
} tle_data_uplink_t; //need to ask why the best practice is to put a "_t" at the end

//CMD_ARO_DATA_UPLINK
typedef struct {
    uint32_t timeTag;
    float rollAngle; //why is there "off nadir" in the google sheets?
    //ARO Callsign?
    char passcorde;
} aro_data_uplink_t;

//CMD_SECONDARY_PAYLOAD_ACTIVATION
typedef struct{

} secondary_payload_activation_t;

// CMD_RTC_SYNC 
typedef struct {
	uint32_t time;
} rtc_sync_t;

//CMD_SECONDARY_PAYLOAD_ACTIVATION_TIME
typedef struct {
    uint32_t timeTag;
} secondary_payload_activation_time_t;

//CMD_CALL_EPS_MCU_RESET
typedef struct {

} call_eps_mcu_reset_t;

//CMD_CALL_CDH_MCU_RESET
typedef struct {

} call_cdh_mcu_reset_t;

//CMD_TIME_DELAY_IMG_CAPTURE_AND_DOWNLINK
typedef struct {
    uint32_t timeTag;
    float rollAngle;
} time_delay_img_capture_and_downlink_t;

//CMD_DIRECT_EPS_POWER_RAIL_CONTROL
typedef struct {
    int powerRailID;
} direct_eps_power_rail_control_t;

//CMD_COMMS_ECHO_BACK
typedef struct {
    uint32_t intEchoBack;
} comms_echo_back_t;

//CMD_CHANGE_VALUE_PARAM_MNGER
typedef struct {
    int newAddress;
} change_value_parm_mnger_t;

//CMD_ECHO_PARM_MNGER_VALUE
typedef struct {
    int address;
} echo_parm_mnger_value_t;

//CMD_MANUALLY_ACTIVATE_EMERGENCY_MODE_SPECIFIED_TIME
typedef struct {
    int emergModeNum;
    uint32_t timeTag;
} manually_activate_emergency_mode_specified_time_t;

//CMD_DOWNLINK_FULL_SYS_LOGS_NEXT_PASS
typedef struct {
    enum logLevel;
} downlink_full_sys_logs_next_pass_t;

/*data structs*/

/*command struct*/
typedef struct {
	union {
		tle_data_uplink_t tleDataUplink;
		aro_data_uplink_t aroDataUplink;
        secondary_payload_activation_t secondaryPayloadActivation;
        rtc_sync_t rtcSync;
        secondary_payload_activation_time_t secondaryPayloadActivationTime;
        call_eps_mcu_reset_t callEPSMCUReset;
        call_cdh_mcu_reset_t callCDHMCUReset;
		time_delay_img_capture_and_downlink_t timeDelayImgCaptureAndDownlink;
        direct_eps_power_rail_control_t directEPSPowerRailControl;
        comms_echo_back_t commsEchoBack;
        change_value_parm_mnger_t changeValueParmMnger;
        echo_parm_mnger_value_t echoParmMngerValue;
        manually_activate_emergency_mode_specified_time_t manuallyActivateEmergencyModeSpecifiedTime;
        downlink_full_sys_logs_next_pass_t downlinkFullSysLogsNextPass;
	};
	uint32_t timestamp;
	uint8_t id;
	bool isTimeTagged;
} cmd_msg_t;

/*command struct*/

//callback functions (Will probably be implemented in other files)
//these are examples of what the callback functions could be used for

void tleDataUplinkCmdCallback( tle_data_uplink_t tleDataUplink ) {
	sendToADCSQueue( tleDataUplink) ;
}

void aroDataUplinkCmdCallback( aro_data_uplink_t aroDataUplink ) {
    sendToADCSQueue( aroDataUplink );
	
}

void secondaryPayloadActivationCmdCallback( secondary_payload_activation_t secondaryPayloadActivation ) {
	sendToADCSQueue( aroDataUplink );
}

void rtcSyncCmdCallback( rtc_sync_t rtcSync ) {
    sendToADCSQueue( aroDataUplink );
}

void secondaryPayloadActivationTimeCmdCallback( secondary_payload_activation_time_t secondaryPayloadActivationTime ) {
    sendToADCSQueue( aroDataUplink );
}

void callEPSMCUResetCmdCallback( call_eps_mcu_reset_t callEPSMCUReset ) {
    sendToADCSQueue( aroDataUplink );
}

void callCDHMCUResetCmdCallback ( call_cdh_mcu_reset_t callCDHMCUReset ) {
    sendToADCSQueue( aroDataUplink );
}

void timeDelayImgCaptureAndDownlinkCmdCallback ( time_delay_img_capture_and_downlink_t timeDelayImgCaptureAndDownlink ) {
    sendToADCSQueue( aroDataUplink );
}

void directEPSPowerRailControlCmdCallback ( direct_eps_power_rail_control_t directEPSPowerRailControl ) {
    sendToADCSQueue( aroDataUplink );
}

void commsEchoBackCmdCallback ( comms_echo_back_t commsEchoBack ) {
    sendToADCSQueue( aroDataUplink );
}

void changeValueParmMngerCmdCallback ( change_value_parm_mnger_t changeValueParmMnger ) {
    sendToADCSQueue( aroDataUplink );
}

void echoParmMngerValueCmdCallback ( echo_parm_mnger_value_t echoParmMngerValue ) {
    sendToADCSQueue( aroDataUplink );
}

void manuallyActivateEmergencyModeSpecifiedTimeCmdCallback ( manually_activate_emergency_mode_specified_time_t manuallyActivateEmergencyModeSpecifiedTime ) {
    sendToADCSQueue( aroDataUplink );
}

void downlinkFullSysLogsNextPassCmdCallback ( downlink_full_sys_logs_next_pass_t downlinkFullSysLogsNextPass ) {
    sendToADCSQueue( aroDataUplink );
}