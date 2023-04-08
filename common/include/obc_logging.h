#ifndef COMMON_INCLUDE_LOGGING_H_
#define COMMON_INCLUDE_LOGGING_H_

#include "obc_errors.h"

#include <stdint.h>
#include <stdarg.h>

/**
 * @enum log_output_location_t
 * @brief Log output location enum.
 * 
 * Enum containing all locations to output logs to.
 */
typedef enum {
	LOG_TO_SDCARD,
	LOG_TO_UART
} log_output_location_t;

#ifndef LOG_DEFAULT_OUTPUT_LOCATION
#define LOG_DEFAULT_OUTPUT_LOCATION LOG_TO_UART
#endif

/**
 * @enum log_level_t
 * @brief Log levels enum.
 * 
 * Enum containing all log levels.
 */
typedef enum {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
	LOG_OFF
} log_level_t;

#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL LOG_TRACE
#endif

#define LOG_TRACE(...)  logLog(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)  logLog(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)   logLog(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)   logLog(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)  logLog(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)  logLog(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_ERROR_CODE(errCode) LOG_ERROR("Error code: %lu", (uint32_t)errCode)

#define RETURN_IF_ERROR_CODE(_ret)  do {                                        \
                                        errCode = _ret;                         \
                                        if (errCode != OBC_ERR_CODE_SUCCESS) {  \
                                            LOG_ERROR_CODE(errCode);            \
                                            return errCode;                     \
                                        }                                       \
                                    } while (0)
                            

#define LOG_IF_ERROR_CODE(_ret) do {                                        \
                                    errCode = _ret;                         \
                                    if (errCode != OBC_ERR_CODE_SUCCESS) {  \
                                        LOG_ERROR_CODE(errCode);            \
                                    }                                       \
                                } while (0)
                                                          
/**
 * @brief Initialize the logger
 */
void initLogger(void);

/**
 * @brief Set the logging level
 * 
 * @param newLogLevel The new logging level
 */
void logSetLevel(log_level_t newLogLevel);

/**
 * @brief Set the output location
 * 
 * @param newOutputLocation The new output location
 */
void logSetOutputLocation(log_output_location_t newOutputLocation);

/**
 * @brief Log a message
 * 
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param s						Message to log
 * @param ...					Additional arguments for the message
 * @return obc_error_code_t		OBC_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_ERR_CODE_INVALID_ARG		if file or s are null or if there is an encoding error
 * 								OBC_ERR_CODE_SUCCESS			if message is successfully logged
 * 								OBC_ERR_CODE_UNKNOWN 			otherwise
 * 								
 */
obc_error_code_t logLog(log_level_t msgLevel, const char *file, uint32_t line, const char *s, ...);

#endif