#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_sci_io.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <redposix.h>

#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
// Extra 10 for the small extra pieces in "%s - %s\r\n"
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

	// Message
	char msgbuf[MAX_MSG_SIZE] = {0};
	va_list args;
	va_start(args, s);
	ret = vsnprintf(msgbuf, MAX_MSG_SIZE, s, args);
	va_end(args);
	if (ret < 0)
		return OBC_ERR_CODE_INVALID_ARG;
	if ((uint32_t)ret >= MAX_MSG_SIZE)
		return OBC_ERR_CODE_BUFF_TOO_SMALL;


	// File & line number
	char infobuf[MAX_FNAME_LINENUM_SIZE] = {0};
	ret = snprintf(infobuf, MAX_FNAME_LINENUM_SIZE, "%-5s -> %s:%lu", LEVEL_STRINGS[msgLevel], file, line);
	if (ret < 0)
		return OBC_ERR_CODE_INVALID_ARG;
	if ((uint32_t)ret >= MAX_FNAME_LINENUM_SIZE)
		return OBC_ERR_CODE_BUFF_TOO_SMALL;

	// Prepare entire output
	char buf[MAX_LOG_SIZE] = {0};
	ret = snprintf(buf, MAX_LOG_SIZE, "%s - %s\r\n", infobuf, msgbuf);
	if (ret < 0)
		return OBC_ERR_CODE_INVALID_ARG;
	if ((uint32_t)ret >= MAX_LOG_SIZE)
		return OBC_ERR_CODE_BUFF_TOO_SMALL;

	if (outputLocation == LOG_TO_UART) {
		obc_error_code_t retSci = sciPrintText((unsigned char *)buf, sizeof(buf));
		return retSci;
	} else if (outputLocation == LOG_TO_SDCARD) {
		const char * fname = *file;
		
		int32_t ret = red_init();
		if (ret != 0) {
			sciPrintf("red_init() returned %d with ERRNO %d\r\n", ret, red_errno);
			while (1);
		}

		ret = red_format("");
		if (ret != 0) {
			if (red_errno != 16) {
				sciPrintf("red_format() returned %d with ERRNO %d\r\n", ret, red_errno);

				while (1);
			}
		}

		ret = red_mount("");
		if (ret != 0) {
			if (red_errno != 16) {
				sciPrintf("red_format() returned %d with ERRNO %d\r\n", ret, red_errno);

				while (1);
			}
		}

		int32_t fdescriptor = red_open(fname, RED_O_RDWR | RED_O_APPEND);
		if (fdescriptor < 0) {
			if (red_errno != 2) {
				sciPrintf("red_open() returned %d with ERRNO %d\r\n", fdescriptor, red_errno);

				while (1);
			}
			else {
				fdescriptor = red_open(fname, RED_O_WRONLY | RED_O_CREAT);
			}
		}

		ret = red_write(fdescriptor, (unsigned char *)buf, sizeof(buf));
		if (ret < 0) {
			sciPrintf("red_write() returned %d with ERRNO %d\r\n", ret, red_errno);
			while (1);
		}

		ret = red_close(fdescriptor);
		if (ret < 0) {
			sciPrintf("red_close() returned %d with ERRNO %d\r\n", ret, red_errno);
			while (1);
		}
	}

	return OBC_ERR_CODE_UNKNOWN;
}