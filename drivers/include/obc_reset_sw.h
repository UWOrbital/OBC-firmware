#ifndef DRIVERS_INCLUDE_OBC_RESET_SW_H_
#define DRIVERS_INCLUDE_OBC_RESET_SW_H_

#define RESET_ADDR_MASK (1 << 15)

/**
 * @brief Reasons for resetting the system
 */
typedef enum{
    //Example reset reason
    RESET_REASON_TESTING = 0

}obc_reset_reason_t;

/**
 * @brief reset the systems
 * 
 * @param obc_reset_reason_t - the reason to reset system
 */
void resetSystem(obc_reset_reason_t reason);

#endif //DRIVERS_INCLUDE_OBC_RESET_SW_H
