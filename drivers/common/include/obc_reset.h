#ifndef DRIVERS_INCLUDE_OBC_RESET_H_
#define DRIVERS_INCLUDE_OBC_RESET_H_

/**
 * @brief Reasons for resetting the system
 */
typedef enum {
    RESET_REASON_TESTING,               // For testing purposes
    RESET_REASON_CMD_EXEC_OBC_RESET,    // Reset due to command execution 
    RESET_REASON_FS_FAILURE,            // File system operation failed
} obc_reset_reason_t;

/**
 * @brief reset the systems
 * 
 * @param obc_reset_reason_t - the reason to reset system
 */
void resetSystem(obc_reset_reason_t reason);

#endif //DRIVERS_INCLUDE_OBC_RESET_H_
