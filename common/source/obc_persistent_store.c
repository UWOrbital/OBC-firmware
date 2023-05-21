#include "obc_persistent_store.h"
#include "fram.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <string.h>
#include <stdbool.h>
#include <redutils.h>

typedef struct {
    size_t len;
    uint32_t crc32;
} fram_header_t;

typedef struct {
    fram_header_t header;
    fram_sys_data_t data;
} fram_sys_persist_t;

typedef struct {
    fram_header_t header;
    fram_adcs_data_t data;
} fram_adcs_persist_t;

typedef struct {
    fram_header_t header;
    fram_comms_data_t data;
} fram_comms_persist_t;

typedef struct {
    fram_header_t header;
    fram_cdh_data_t data;
} fram_cdh_persist_t;

typedef struct {
    fram_header_t header;
    fram_eps_data_t data;
} fram_eps_persist_t;

typedef struct {
    fram_header_t header;
    fram_payload_data_t data;
} fram_payload_persist_t;

typedef struct {
    fram_sys_persist_t system_data;
    fram_adcs_persist_t adcs_data;
    fram_comms_persist_t comms_data;
    fram_cdh_persist_t cdh_data;
    fram_eps_persist_t eps_data;
    fram_payload_data_t payload_data;
} fram_persist_t;

#define FRAM_ADDRESS_OF(data)  (0x0 + offsetof(fram_persist_t, data))

// #define FRAM_SYS_ADDR   (0x0 + offsetof(fram_persist_t, system_data))
// #define FRAM_ADCS_ADDR  (0x0 + offsetof(fram_persist_t, adcs_data))
// #define FRAM_COMMS_ADDR (0x0 + offsetof(fram_persist_t, comms_data))
// #define FRAM_CDH_ADDR   (0x0 + offsetof(fram_persist_t, cdh_data))
// #define FRAM_EPS_ADDR   (0x0 + offsetof(fram_persist_t, eps_data))

obc_error_code_t getPersistentSysData(fram_sys_data_t *buffer) {
    obc_error_code_t errCode;

    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t fram_address = FRAM_ADDRESS_OF(system_data);

    //Read FRAM
    uint8_t read_buffer[sizeof(fram_sys_persist_t)] = {0};
    RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_sys_persist_t)));

    //Parse Stored data
    fram_sys_persist_t read_data = {0};
    memcpy(&read_data, read_buffer, sizeof(fram_sys_persist_t));
    
    //Integrity Check
    uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_sys_data_t));
    if(read_data.header.len != sizeof(fram_sys_data_t)){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    if(read_data.header.crc32 != crc32){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    memcpy(buffer, &read_data.data, sizeof(fram_sys_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentSysData(fram_sys_data_t data) {
    obc_error_code_t errCode;
    uint32_t fram_address = FRAM_ADDRESS_OF(system_data);;
    fram_sys_persist_t write_data = {0};
    uint8_t write_buffer[sizeof(fram_sys_persist_t)] = {0};

    write_data.header.len = sizeof(fram_sys_data_t);
    //Use CRC32 function from Red to calculate crc
    write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_sys_data_t));
    write_data.data = data;
    memcpy(write_buffer, &write_data, sizeof(fram_sys_persist_t));

    RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_sys_persist_t)));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentADCSData(fram_adcs_data_t *buffer) {
    obc_error_code_t errCode;

    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t fram_address = FRAM_ADDRESS_OF(adcs_data);

    //Read FRAM
    uint8_t read_buffer[sizeof(fram_adcs_persist_t)] = {0};
    RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_adcs_persist_t)));

    //Parse Stored data
    fram_adcs_persist_t read_data = {0};
    memcpy(&read_data, read_buffer, sizeof(fram_adcs_persist_t));
    
    //Integrity Check
    uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_adcs_data_t));
    if(read_data.header.len != sizeof(fram_adcs_data_t)){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    if(read_data.header.crc32 != crc32){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    memcpy(buffer, &read_data.data, sizeof(fram_adcs_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentADCSData(fram_adcs_data_t data) {
    obc_error_code_t errCode;
    uint32_t fram_address = FRAM_ADDRESS_OF(adcs_data);
    fram_adcs_persist_t write_data = {0};
    uint8_t write_buffer[sizeof(fram_adcs_persist_t)] = {0};

    write_data.header.len = sizeof(fram_adcs_data_t);
    //Use CRC32 function from Red to calculate crc
    write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_adcs_data_t));
    write_data.data = data;
    memcpy(write_buffer, &write_data, sizeof(fram_adcs_persist_t));

    RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_adcs_persist_t)));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentCOMMSData(fram_comms_data_t *buffer) {
    obc_error_code_t errCode;

    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t fram_address = FRAM_ADDRESS_OF(comms_data);

    //Read FRAM
    uint8_t read_buffer[sizeof(fram_comms_persist_t)] = {0};
    RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_comms_persist_t)));

    //Parse Stored data
    fram_comms_persist_t read_data = {0};
    memcpy(&read_data, read_buffer, sizeof(fram_comms_persist_t));
    
    //Integrity Check
    uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_comms_data_t));
    if(read_data.header.len != sizeof(fram_comms_data_t)){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    if(read_data.header.crc32 != crc32){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    memcpy(buffer, &read_data.data, sizeof(fram_comms_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentCOMMSData(fram_comms_data_t data) {
    obc_error_code_t errCode;
    uint32_t fram_address = FRAM_ADDRESS_OF(comms_data);
    fram_comms_persist_t write_data = {0};
    uint8_t write_buffer[sizeof(fram_comms_persist_t)] = {0};

    write_data.header.len = sizeof(fram_comms_data_t);
    //Use CRC32 function from Red to calculate crc
    write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_comms_data_t));
    write_data.data = data;
    memcpy(write_buffer, &write_data, sizeof(fram_comms_persist_t));

    RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_comms_persist_t)));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentCDHData(fram_cdh_data_t *buffer) {
    obc_error_code_t errCode;

    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t fram_address = FRAM_ADDRESS_OF(cdh_data);

    //Read FRAM
    uint8_t read_buffer[sizeof(fram_cdh_persist_t)] = {0};
    RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_cdh_persist_t)));

    //Parse Stored data
    fram_cdh_persist_t read_data = {0};
    memcpy(&read_data, read_buffer, sizeof(fram_cdh_persist_t));
    
    //Integrity Check
    uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_cdh_data_t));
    if(read_data.header.len != sizeof(fram_cdh_data_t)){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    if(read_data.header.crc32 != crc32){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    memcpy(buffer, &read_data.data, sizeof(fram_cdh_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentCDHData(fram_cdh_data_t data) {
    obc_error_code_t errCode;
    uint32_t fram_address = FRAM_ADDRESS_OF(cdh_data);
    fram_cdh_persist_t write_data = {0};
    uint8_t write_buffer[sizeof(fram_cdh_persist_t)] = {0};

    write_data.header.len = sizeof(fram_cdh_data_t);
    //Use CRC32 function from Red to calculate crc
    write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_cdh_data_t));
    write_data.data = data;
    memcpy(write_buffer, &write_data, sizeof(fram_cdh_persist_t));

    RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_cdh_persist_t)));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentEPSData(fram_eps_data_t *buffer) {
    obc_error_code_t errCode;

    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t fram_address = FRAM_ADDRESS_OF(eps_data);

    //Read FRAM
    uint8_t read_buffer[sizeof(fram_eps_persist_t)] = {0};
    RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_eps_persist_t)));

    //Parse Stored data
    fram_eps_persist_t read_data = {0};
    memcpy(&read_data, read_buffer, sizeof(fram_eps_persist_t));
    
    
    //Integrity Check
    uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_eps_data_t));
    if(read_data.header.len != sizeof(fram_eps_data_t)){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    if(read_data.header.crc32 != crc32){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    memcpy(buffer, &read_data.data, sizeof(fram_eps_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentEPSData(fram_eps_data_t data) {
    obc_error_code_t errCode;
    uint32_t fram_address = FRAM_ADDRESS_OF(eps_data);
    fram_eps_persist_t write_data = {0};
    uint8_t write_buffer[sizeof(fram_eps_persist_t)] = {0};

    write_data.header.len = sizeof(fram_eps_data_t);
    //Use CRC32 function from Red to calculate crc
    write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_eps_data_t));
    write_data.data = data;
    memcpy(write_buffer, &write_data, sizeof(fram_eps_persist_t));

    RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_eps_persist_t)));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentPayloadData(fram_payload_data_t *buffer) {
    obc_error_code_t errCode;

    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t fram_address = FRAM_ADDRESS_OF(payload_data);

    //Read FRAM
    uint8_t read_buffer[sizeof(fram_payload_persist_t)] = {0};
    RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_payload_persist_t)));

    //Parse Stored data
    fram_payload_persist_t read_data = {0};
    memcpy(&read_data, read_buffer, sizeof(fram_payload_persist_t));
    
    
    //Integrity Check
    uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_payload_data_t));
    if(read_data.header.len != sizeof(fram_payload_data_t)){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    if(read_data.header.crc32 != crc32){
        // Do something
        return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    }

    memcpy(buffer, &read_data.data, sizeof(fram_payload_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentPayloadData(fram_payload_data_t data) {
    obc_error_code_t errCode;
    uint32_t fram_address = FRAM_ADDRESS_OF(payload_data);
    fram_payload_persist_t write_data = {0};
    uint8_t write_buffer[sizeof(fram_payload_persist_t)] = {0};

    write_data.header.len = sizeof(fram_payload_data_t);
    //Use CRC32 function from Red to calculate crc
    write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_payload_data_t));
    write_data.data = data;
    memcpy(write_buffer, &write_data, sizeof(fram_payload_persist_t));

    RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_payload_persist_t)));
    return OBC_ERR_CODE_SUCCESS;
}