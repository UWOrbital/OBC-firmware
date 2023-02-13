#ifndef DRIVERS_INCLUDE_OBC_RESET_SW_H_
#define DRIVERS_INCLUDE_OBC_RESET_SW_H_

#define RESET_ADDR_MASK (1 << 15)

typedef enum{
    //Example reset reason
    RESET_REASON_TESTIG = 0

}obc_reset_reason_t;

#endif //DRIVERS_INCLUDE_OBC_RESET_SW_H