#ifndef COMMON_INCLUDE_OBC_ERRORS_H_
#define COMMON_INCLUDE_OBC_ERRORS_H_

typedef enum {
    /* Common Errors 0 - 99 */
    OBC_ERR_CODE_SUCCESS = 0,
    OBC_ERR_CODE_UNKNOWN = 1,
    OBC_ERR_CODE_INVALID_ARG = 2,
    OBC_ERR_CODE_QUEUE_FULL = 3,
    OBC_ERR_CODE_MUTEX_TIMEOUT = 4,
    OBC_ERR_CODE_BUFF_TOO_SMALL = 5,
    OBC_ERR_CODE_LOG_MSG_SILENCED = 6,
    OBC_ERR_CODE_INVALID_STATE = 7,
    OBC_ERR_CODE_UNSUPPORTED_EVENT = 8,
    OBC_ERR_CODE_BUFF_OVERFLOW = 9,
    OBC_ERR_CODE_SEMAPHORE_TIMEOUT = 10,
    OBC_ERR_CODE_SEMAPHORE_FULL = 11,
    OBC_ERR_CODE_QUEUE_EMPTY = 12,
    OBC_ERR_CODE_NOT_MUTEX_OWNER = 13,
    OBC_ERR_CODE_PERSISTENT_CORRUPTED = 14,
    
    /* Driver Errors 100 - 199*/
    OBC_ERR_CODE_SPI_FAILURE = 100,
    OBC_ERR_CODE_I2C_FAILURE = 101,
    OBC_ERR_CODE_UART_FAILURE = 102,
    OBC_ERR_CODE_FRAM_ADDRESS_OUT_OF_RANGE = 103,
    OBC_ERR_CODE_I2C_TRANSFER_TIMEOUT = 104,

    /* CDH errors 200 - 299 */
    OBC_ERR_CODE_UNSUPPORTED_CMD = 200,
    OBC_ERR_CODE_CMD_NOT_ALLOWED = 201,

    /* ADCS errors 300 - 399 */


    /* EPS errors 400 - 499 */


    /* COMMS errors 500 - 599 */
    OBC_ERR_CODE_CC1120_READ_EXT_ADDR_SPI_FAILED = 500,
    OBC_ERR_CODE_CC1120_WRITE_EXT_ADDR_SPI_FAILED,
    OBC_ERR_CODE_CC1120_INVALID_STATUS_BYTE,
    OBC_ERR_CODE_CORRUPTED_MSG,
    OBC_ERR_CODE_CC1120_RECEIVE_FAILURE,
    OBC_ERR_CODE_CC1120_TEST_FAILURE = 599,
    

    /* Payload errors 600 - 699 */
    OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE = 600,

    /* File System errors 700 - 799 */
    OBC_ERR_CODE_INVALID_FILE_NAME = 700,
    OBC_ERR_CODE_FAILED_FILE_OPEN = 701,
    OBC_ERR_CODE_FAILED_FILE_CLOSE = 702,
    OBC_ERR_CODE_FAILED_FILE_WRITE = 703,
    OBC_ERR_CODE_FAILED_FILE_READ = 704,
    OBC_ERR_CODE_REACHED_EOF = 705,
    OBC_ERR_CODE_MAX_FILE_SIZE_REACHED = 706,
    OBC_ERR_CODE_FS_INIT_FAILED = 707,
    OBC_ERR_CODE_FS_MOUNT_FAILED = 708,
    OBC_ERR_CODE_FS_FORMAT_FAILED = 709,
    OBC_ERR_CODE_MKDIR_FAILED = 710,
    OBC_ERR_CODE_FAILED_FILE_DELETE = 711,
    OBC_ERR_CODE_FAILED_FILE_SEEK = 712,

    /* Time errors 800 - 899 */
    OBC_ERR_CODE_UNSUPPORTED_ALARM_TYPE = 800,
    OBC_ERR_CODE_RTC_ALARM_EARLY = 801,

} obc_error_code_t;


#endif // COMMON_INCLUDE_OBC_ERRORS_H_