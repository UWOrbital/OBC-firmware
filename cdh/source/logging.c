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

void initLogger(){
	logLevel = LOG_DEFAULT_LEVEL;
}

void logSetLevel(log_level_t newLogLevel){
	logLevel = newLogLevel;
}

void logLog(log_level_t msgLevel, const char *file, int line, const char *s, ...){
	if (msgLevel < logLevel)
		return;

	time_t t = time(NULL);
	char timebuf[32];
	strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&t));

	char infobuf[64];
	snprintf(infobuf, sizeof(infobuf), "%-5s -> %s:%d", LEVEL_STRINGS[msgLevel], file, line);

	va_list args;
	va_start(args, s);
	char msgbuf[MAX_MSG_SIZE];
	vsnprintf(msgbuf, sizeof(msgbuf), s, args);
	va_end(args);

	char buf[MAX_MSG_SIZE + 128];
	snprintf(buf, sizeof(buf), "%s %s - %s\r\n", timebuf, infobuf, msgbuf);

	if (LOG_OUTPUT_LOCATION == LOG_TO_UART){
		sciPrintText((unsigned char *)buf, sizeof(buf));
	}
	else if (LOG_OUTPUT_LOCATION == LOG_TO_SDCARD){
		// implement when SD card driver is written
	}
}