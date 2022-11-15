#ifndef CDH_INCLUDE_LOGGING_H_
#define CDH_INCLUDE_LOGGING_H_

#include <sys_common.h>

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

#define LOG_OUTPUT_LOCATION LOG_TO_UART

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

#define LOG_DEFAULT_LEVEL LOG_WARN

#define LOG_TRACE(s, ...)  logLog(LOG_TRACE, __FILE__, __LINE__, s, ...)
#define LOG_DEBUG(s, ...)  logLog(LOG_DEBUG, __FILE__, __LINE__, s, ...)
#define LOG_INFO(s, ...)   logLog(LOG_INFO, __FILE__, __LINE__, s, ...)
#define LOG_WARN(s, ...)   logLog(LOG_WARN, __FILE__, __LINE__, s, ...)
#define LOG_ERROR(s, ...)  logLog(LOG_ERROR, __FILE__, __LINE__, s, ...)
#define LOG_FATAL(s, ...)  logLog(LOG_FATAL, __FILE__, __LINE__, s, ...)

/**
 * @brief Set the logging level
 * 
 * @param 
 */
void logSetLevel(log_level_t newLogLevel);

/**
 * @brief Log a message
 * 
 * @param msgLevel  Level of the message
 * @param file      File of message
 * @param line      Line of message
 * @param s         Message to log
 * @param ...       Additional arguments for the message
 */
void logLog(log_level_t msgLevel, const char *file, int line, const char *s, ...);

#endif