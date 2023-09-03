#include "obc_persistent_store.h"

#include "fm25v20a.h"
#include "obc_assert.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <stdbool.h>
#include <redutils.h>
#include <string.h>

typedef struct {
  size_t len;
  uint32_t crc32;
} fram_header_t;

typedef struct {
  fram_header_t header;
  fram_time_data_t data;
} fram_time_persist_t;

typedef struct {
  fram_header_t header;
  fram_state_data_t data;
} fram_state_persist_t;

// typedef struct {
//     fram_header_t header;
//     fram_adcs_data_t data;
// } fram_adcs_persist_t;

// typedef struct {
//     fram_header_t header;
//     fram_comms_data_t data;
// } fram_comms_persist_t;

typedef struct {
  fram_header_t header;
  fram_cdh_data_t data;
} fram_cdh_persist_t;

// typedef struct {
//     fram_header_t header;
//     fram_eps_data_t data;
// } fram_eps_persist_t;

// typedef struct {
//     fram_header_t header;
//     fram_payload_data_t data;
// } fram_payload_persist_t;

typedef struct {
  fram_time_persist_t time_data;
  fram_state_data_t state_data;
  // fram_adcs_persist_t adcs_data;
  // fram_comms_persist_t comms_data;
  fram_cdh_persist_t cdh_data;
  // fram_eps_persist_t eps_data;
  // fram_payload_data_t payload_data;
} fram_persist_t;

STATIC_ASSERT(sizeof(fram_persist_t) <= FRAM_MAX_ADDRESS, "Data stored too large for FRAM");

#define FRAM_ADDRESS_OF(data) (0x0 + offsetof(fram_persist_t, data))

obc_error_code_t getPersistentTimeData(fram_time_data_t *buffer) {
  obc_error_code_t errCode;

  if (buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint32_t fram_address = FRAM_ADDRESS_OF(time_data);

  // Read FRAM
  uint8_t read_buffer[sizeof(fram_time_persist_t)] = {0};
  RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_time_persist_t)));

  // Parse Stored data
  fram_time_persist_t read_data = {0};
  memcpy(&read_data, read_buffer, sizeof(fram_time_persist_t));

  // Integrity Check
  uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_time_data_t));
  if (read_data.header.len != sizeof(fram_time_data_t)) {
    // Do something
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  if (read_data.header.crc32 != crc32) {
    // Do something
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  memcpy(buffer, &read_data.data, sizeof(fram_time_data_t));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentTimeData(fram_time_data_t data) {
  obc_error_code_t errCode;
  uint32_t fram_address = FRAM_ADDRESS_OF(time_data);
  ;
  fram_time_persist_t write_data = {0};
  uint8_t write_buffer[sizeof(fram_time_persist_t)] = {0};

  write_data.header.len = sizeof(fram_time_data_t);
  // Use CRC32 function from Red to calculate crc
  write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_time_data_t));
  write_data.data = data;
  memcpy(write_buffer, &write_data, sizeof(fram_time_persist_t));

  RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_time_persist_t)));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getPersistentStateData(fram_state_data_t *buffer) {
  obc_error_code_t errCode;

  if (buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint32_t fram_address = FRAM_ADDRESS_OF(state_data);

  // Read FRAM
  uint8_t read_buffer[sizeof(fram_state_persist_t)] = {0};
  RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_state_persist_t)));

  // Parse Stored data
  fram_state_persist_t read_data = {0};
  memcpy(&read_data, read_buffer, sizeof(fram_state_persist_t));

  // Integrity Check
  uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_state_data_t));
  if (read_data.header.len != sizeof(fram_state_data_t)) {
    // Do something
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  if (read_data.header.crc32 != crc32) {
    // Do something
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  memcpy(buffer, &read_data.data, sizeof(fram_state_data_t));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistentStateData(fram_state_data_t data) {
  obc_error_code_t errCode;
  uint32_t fram_address = FRAM_ADDRESS_OF(state_data);
  ;
  fram_state_persist_t write_data = {0};
  uint8_t write_buffer[sizeof(fram_state_persist_t)] = {0};

  write_data.header.len = sizeof(fram_state_data_t);
  // Use CRC32 function from Red to calculate crc
  write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_state_data_t));
  write_data.data = data;
  memcpy(write_buffer, &write_data, sizeof(fram_state_persist_t));

  RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_state_persist_t)));
  return OBC_ERR_CODE_SUCCESS;
}

// obc_error_code_t getPersistentADCSData(fram_adcs_data_t *buffer) {
//     obc_error_code_t errCode;

//     if (buffer == NULL) {
//         return OBC_ERR_CODE_INVALID_ARG;
//     }

//     uint32_t fram_address = FRAM_ADDRESS_OF(adcs_data);

//     //Read FRAM
//     uint8_t read_buffer[sizeof(fram_adcs_persist_t)] = {0};
//     RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_adcs_persist_t)));

//     //Parse Stored data
//     fram_adcs_persist_t read_data = {0};
//     memcpy(&read_data, read_buffer, sizeof(fram_adcs_persist_t));

//     //Integrity Check
//     uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_adcs_data_t));
//     if (read_data.header.len != sizeof(fram_adcs_data_t)) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     if (read_data.header.crc32 != crc32) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     memcpy(buffer, &read_data.data, sizeof(fram_adcs_data_t));
//     return OBC_ERR_CODE_SUCCESS;
// }

// obc_error_code_t setPersistentADCSData(fram_adcs_data_t data) {
//     obc_error_code_t errCode;
//     uint32_t fram_address = FRAM_ADDRESS_OF(adcs_data);
//     fram_adcs_persist_t write_data = {0};
//     uint8_t write_buffer[sizeof(fram_adcs_persist_t)] = {0};

//     write_data.header.len = sizeof(fram_adcs_data_t);
//     //Use CRC32 function from Red to calculate crc
//     write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_adcs_data_t));
//     write_data.data = data;
//     memcpy(write_buffer, &write_data, sizeof(fram_adcs_persist_t));

//     RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_adcs_persist_t)));
//     return OBC_ERR_CODE_SUCCESS;
// }

// obc_error_code_t getPersistentCOMMSData(fram_comms_data_t *buffer) {
//     obc_error_code_t errCode;

//     if (buffer == NULL) {
//         return OBC_ERR_CODE_INVALID_ARG;
//     }

//     uint32_t fram_address = FRAM_ADDRESS_OF(comms_data);

//     //Read FRAM
//     uint8_t read_buffer[sizeof(fram_comms_persist_t)] = {0};
//     RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_comms_persist_t)));

//     //Parse Stored data
//     fram_comms_persist_t read_data = {0};
//     memcpy(&read_data, read_buffer, sizeof(fram_comms_persist_t));

//     //Integrity Check
//     uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_comms_data_t));
//     if (read_data.header.len != sizeof(fram_comms_data_t)) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     if (read_data.header.crc32 != crc32) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     memcpy(buffer, &read_data.data, sizeof(fram_comms_data_t));
//     return OBC_ERR_CODE_SUCCESS;
// }

// obc_error_code_t setPersistentCOMMSData(fram_comms_data_t data) {
//     obc_error_code_t errCode;
//     uint32_t fram_address = FRAM_ADDRESS_OF(comms_data);
//     fram_comms_persist_t write_data = {0};
//     uint8_t write_buffer[sizeof(fram_comms_persist_t)] = {0};

//     write_data.header.len = sizeof(fram_comms_data_t);
//     //Use CRC32 function from Red to calculate crc
//     write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_comms_data_t));
//     write_data.data = data;
//     memcpy(write_buffer, &write_data, sizeof(fram_comms_persist_t));

//     RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_comms_persist_t)));
//     return OBC_ERR_CODE_SUCCESS;
// }

obc_error_code_t getPersistentCDHData(fram_cdh_data_t *buffer) {
  obc_error_code_t errCode;

  if (buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint32_t fram_address = FRAM_ADDRESS_OF(cdh_data);

  // Read FRAM
  uint8_t read_buffer[sizeof(fram_cdh_persist_t)] = {0};
  RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_cdh_persist_t)));

  // Parse Stored data
  fram_cdh_persist_t read_data = {0};
  memcpy(&read_data, read_buffer, sizeof(fram_cdh_persist_t));

  // Integrity Check
  uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_cdh_data_t));
  if (read_data.header.len != sizeof(fram_cdh_data_t)) {
    // Do something
    return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
  }

  if (read_data.header.crc32 != crc32) {
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
  // Use CRC32 function from Red to calculate crc
  write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_cdh_data_t));
  write_data.data = data;
  memcpy(write_buffer, &write_data, sizeof(fram_cdh_persist_t));

  RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_cdh_persist_t)));
  return OBC_ERR_CODE_SUCCESS;
}

// obc_error_code_t getPersistentEPSData(fram_eps_data_t *buffer) {
//     obc_error_code_t errCode;

//     if (buffer == NULL) {
//         return OBC_ERR_CODE_INVALID_ARG;
//     }

//     uint32_t fram_address = FRAM_ADDRESS_OF(eps_data);

//     //Read FRAM
//     uint8_t read_buffer[sizeof(fram_eps_persist_t)] = {0};
//     RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_eps_persist_t)));

//     //Parse Stored data
//     fram_eps_persist_t read_data = {0};
//     memcpy(&read_data, read_buffer, sizeof(fram_eps_persist_t));

//     //Integrity Check
//     uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_eps_data_t));
//     if (read_data.header.len != sizeof(fram_eps_data_t)) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     if (read_data.header.crc32 != crc32) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     memcpy(buffer, &read_data.data, sizeof(fram_eps_data_t));
//     return OBC_ERR_CODE_SUCCESS;
// }

// obc_error_code_t setPersistentEPSData(fram_eps_data_t data) {
//     obc_error_code_t errCode;
//     uint32_t fram_address = FRAM_ADDRESS_OF(eps_data);
//     fram_eps_persist_t write_data = {0};
//     uint8_t write_buffer[sizeof(fram_eps_persist_t)] = {0};

//     write_data.header.len = sizeof(fram_eps_data_t);
//     //Use CRC32 function from Red to calculate crc
//     write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_eps_data_t));
//     write_data.data = data;
//     memcpy(write_buffer, &write_data, sizeof(fram_eps_persist_t));

//     RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_eps_persist_t)));
//     return OBC_ERR_CODE_SUCCESS;
// }

// obc_error_code_t getPersistentPayloadData(fram_payload_data_t *buffer) {
//     obc_error_code_t errCode;

//     if (buffer == NULL) {
//         return OBC_ERR_CODE_INVALID_ARG;
//     }

//     uint32_t fram_address = FRAM_ADDRESS_OF(payload_data);

//     //Read FRAM
//     uint8_t read_buffer[sizeof(fram_payload_persist_t)] = {0};
//     RETURN_IF_ERROR_CODE(framRead(fram_address, read_buffer, sizeof(fram_payload_persist_t)));

//     //Parse Stored data
//     fram_payload_persist_t read_data = {0};
//     memcpy(&read_data, read_buffer, sizeof(fram_payload_persist_t));

//     //Integrity Check
//     uint32_t crc32 = RedCrc32Update(0, &read_data.data, sizeof(fram_payload_data_t));
//     if (read_data.header.len != sizeof(fram_payload_data_t)) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     if (read_data.header.crc32 != crc32) {
//         // Do something
//         return OBC_ERR_CODE_PERSISTENT_CORRUPTED;
//     }

//     memcpy(buffer, &read_data.data, sizeof(fram_payload_data_t));
//     return OBC_ERR_CODE_SUCCESS;
// }

// obc_error_code_t setPersistentPayloadData(fram_payload_data_t data) {
//     obc_error_code_t errCode;
//     uint32_t fram_address = FRAM_ADDRESS_OF(payload_data);
//     fram_payload_persist_t write_data = {0};
//     uint8_t write_buffer[sizeof(fram_payload_persist_t)] = {0};

//     write_data.header.len = sizeof(fram_payload_data_t);
//     //Use CRC32 function from Red to calculate crc
//     write_data.header.crc32 = RedCrc32Update(0, &data, sizeof(fram_payload_data_t));
//     write_data.data = data;
//     memcpy(write_buffer, &write_data, sizeof(fram_payload_persist_t));

//     RETURN_IF_ERROR_CODE(framWrite(fram_address, write_buffer, sizeof(fram_payload_persist_t)));
//     return OBC_ERR_CODE_SUCCESS;
// }
