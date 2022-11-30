#include "logging.h"
#include "obc_errors.h"
#include "obc_sci_io.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
#define MAX_LOG_SIZE (MAX_MSG_SIZE + MAX_FNAME_LINENUM_SIZE + 10U)

static const char *LEVEL_STRINGS[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static log_level_t logLevel;
static log_output_location_t outputLocation;

void initLogger(void){
	logLevel = LOG_DEFAULT_LEVEL;
	outputLocation = LOG_DEFAULT_OUTPUT_LOCATION;
}

void logSetLevel(log_level_t newLogLevel){
	logLevel = newLogLevel;
}

void logSetOutputLocation(log_output_location_t newOutputLocation){
	outputLocation = newOutputLocation;
}

obc_error_code_t logLog(log_level_t msgLevel, const char *file, uint32_t line, const char *s, ...){
	if (msgLevel < logLevel)
		return OBC_ERR_CODE_LOG_MSG_SILENCED;

	if (file == NULL || s == NULL)
		return OBC_ERR_CODE_INVALID_ARG;

	int ret = 0;

	// File & line number
	char infobuf[MAX_FNAME_LINENUM_SIZE];
	ret = snprintf(infobuf, sizeof(infobuf), "%-5s -> %s:%d", LEVEL_STRINGS[msgLevel], file, line);
	if (ret < 0)
		return OBC_ERR_CODE_INVALID_ARG;
	if (ret >= MAX_FNAME_LINENUM_SIZE)
		return OBC_ERR_CODE_BUFF_TOO_SMALL;

	// Message
	va_list args;
	va_start(args, s);
	char msgbuf[MAX_MSG_SIZE];
	ret = vsnprintf(msgbuf, sizeof(msgbuf), s, args);
	va_end(args);
	if (ret < 0)
		return OBC_ERR_CODE_INVALID_ARG;
	if (ret >= MAX_MSG_SIZE)
		return OBC_ERR_CODE_BUFF_TOO_SMALL;

	// Prepare entire output
	char buf[MAX_LOG_SIZE];
	ret = snprintf(buf, sizeof(buf), "%s - %s\r\n", infobuf, msgbuf);
	if (ret < 0)
		return OBC_ERR_CODE_INVALID_ARG;
	if (ret >= MAX_LOG_SIZE)
		return OBC_ERR_CODE_BUFF_TOO_SMALL;

	if (outputLocation == LOG_TO_UART){
		uint8_t retSci = sciPrintText((unsigned char *)buf, sizeof(buf));
		if (retSci)
			return OBC_ERR_CODE_SUCCESS;
	}
	else if (outputLocation == LOG_TO_SDCARD){
		// implement when SD card driver is written
	}

	return OBC_ERR_CODE_UNKNOWN;
}