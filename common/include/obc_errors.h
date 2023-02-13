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

    /* Driver Errors 100 - 199*/
    OBC_ERR_CODE_SPI_FAILURE = 100,
    OBC_ERR_CODE_FRAM_ADDRESS_OUT_OF_RANGE = 101,
    /* CDH errors 200 - 299 */


    /* ADCS errors 300 - 399 */


    /* EPS errors 400 - 499 */


    /* COMMS errors 500 - 599 */


    /* Payload errors 600 - 699 */


} obc_error_code_t;


#endif // COMMON_INCLUDE_OBC_ERRORS_H_