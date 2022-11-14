#ifndef COMMON_INCLUDE_OBC_ERRORS_H_
#define COMMON_INCLUDE_OBC_ERRORS_H_

typedef enum {
    /* Common Errors 0 - 99 */
    OBC_ERR_CODE_SUCCESS = 0,
    OBC_ERR_CODE_UNKOWN = 1,
    OBC_ERR_CODE_INVALID_ARG = 2,
    OBC_ERR_CODE_QUEUE_FULL = 3,

    /* Driver Errors 100 - 199*/


    /* CDH errors 200 - 299 */


    /* ADCS errors 300 - 399 */


    /* EPS errors 400 - 499 */


    /* COMMS errors 500 - 599 */


    /* Payload errors 600 - 699 */


} obc_error_code_t;


#endif // COMMON_INCLUDE_OBC_ERRORS_H_