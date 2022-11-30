#include "logging.h"
#include "obc_sci_io.h"

#include <sci.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define MAX_MSG_SIZE 128U

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

uint8_t logLog(log_level_t msgLevel, const char *file, int line, const char *s, ...){
	if (msgLevel < logLevel)
		return 0;

	if (file == NULL || s == NULL)
		return 0;

	// Time info
#ifdef LOG_TIMESTAMPS
	// rewrite once drivers for external RTC are created
	time_t t = time(NULL);
	char timebuf[32];
	strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&t));
#endif

	uint8_t ret = 0;

	// File & line number
	char infobuf[64];
	ret = snprintf(infobuf, sizeof(infobuf), "%-5s -> %s:%d", LEVEL_STRINGS[msgLevel], file, line);
	if (ret < 0)
		return 0;

	// Message
	va_list args;
	va_start(args, s);
	char msgbuf[MAX_MSG_SIZE];
	ret = vsnprintf(msgbuf, sizeof(msgbuf), s, args);
	va_end(args);
	if (ret < 0)
		return 0 ;

	// Prepare entire output
	char buf[MAX_MSG_SIZE + 128];
#ifdef LOG_TIMESTAMPS
	ret = snprintf(buf, sizeof(buf), "%s %s - %s\r\n", timebuf, infobuf, msgbuf);
#else
	ret = snprintf(buf, sizeof(buf), "%s - %s\r\n", infobuf, msgbuf);
#endif
	if (ret < 0)
		return 0;

	if (outputLocation == LOG_TO_UART){
		ret = sciPrintText((unsigned char *)buf, sizeof(buf));
		return ret;
	}
	else if (outputLocation == LOG_TO_SDCARD){
		// implement when SD card driver is written
	}

	return 0;
}