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

    /* Driver Errors 100 - 199*/
    OBC_ERR_CODE_SPI_FAILURE = 100,
    OBC_ERR_CODE_I2C_FAILURE = 101,
    OBC_ERR_CODE_UART_FAILURE = 102,
    OBC_ERR_CODE_FRAM_ADDRESS_OUT_OF_RANGE = 103,

    /* CDH errors 200 - 299 */    


    /* ADCS errors 300 - 399 */


    /* EPS errors 400 - 499 */


    /* COMMS errors 500 - 599 */
    OBC_ERR_CODE_CC1120_READ_EXT_ADDR_SPI_FAILED = 500,
    OBC_ERR_CODE_CC1120_WRITE_EXT_ADDR_SPI_FAILED,
    OBC_ERR_CODE_CC1120_INVALID_STATUS_BYTE,
    OBC_ERR_CODE_CC1120_SEMAPHORE_TIMEOUT,
    OBC_ERR_CODE_CC1120_TEST_FAILURE = 599,

    /* Payload errors 600 - 699 */

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

} obc_error_code_t;


#endif // COMMON_INCLUDE_OBC_ERRORS_H_