#include "cdh_eps_pack.h"

#include <stddef.h>
#include <stdint.h>

void packCmdMsg(cdh_eps_queue_msg_t *msg, uint8_t id, uint8_t param[7]) {
    msg->cmd.id = id;
    for(uint8_t i = 0; i < 7; i++) {
        msg->cmd.param[i] = param[i];
    }
}

void packTleMsg(cdh_eps_queue_msg_t *msg, uint8_t id, uint8_t data[2]) {
    msg->tle.id = id;
    for(uint8_t i = 0; i < 5; i++) {
        msg->tle.reserved[i] = 0;
    }
    for(uint8_t i = 0; i < 7; i++) {
        msg->tle.data[i] = data[i];
    }
}

void packRespMsg(cdh_eps_queue_msg_t *msg, uint8_t id, uint8_t request) {
    msg->resp.id = id;
    for(uint8_t i = 0; i < 6; i++) {
        msg->tle.reserved[i] = 0;
    }
    msg->resp.request = request;
}