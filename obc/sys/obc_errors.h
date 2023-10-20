#pragma once

#define RELIANCE_EDGE_ERROR_CODES_OFFSET 1000U

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
  OBC_ERR_CODE_FAILED_UNPACK = 15,
  OBC_ERR_CODE_FAILED_PACK = 16,
  OBC_ERR_CODE_INVALID_STATE_TRANSITION = 17,

  /* Driver Errors 100 - 199*/
  OBC_ERR_CODE_SPI_FAILURE = 100,
  OBC_ERR_CODE_I2C_FAILURE = 101,
  OBC_ERR_CODE_UART_FAILURE = 102,
  OBC_ERR_CODE_FRAM_ADDRESS_OUT_OF_RANGE = 103,
  OBC_ERR_CODE_I2C_TRANSFER_TIMEOUT = 104,
  OBC_ERR_CODE_MAX5360_SHUTDOWN_FAILURE = 105,
  OBC_ERR_CODE_FRAM_IS_ASLEEP = 106,
  OBC_ERR_CODE_SD_CARD_INVALID_VOLTAGE = 107,
  OBC_ERR_CODE_SD_CARD_INIT_FAILED = 108,
  OBC_ERR_CODE_SPI_DATA_LENGTH_ERROR = 109,
  OBC_ERR_CODE_SPI_TIMEOUT = 110,
  OBC_ERR_CODE_SPI_PARITY_ERROR = 111,
  OBC_ERR_CODE_SPI_DESYNC_ERROR = 112,
  OBC_ERR_CODE_SPI_BIT_ERROR = 113,
  OBC_ERR_CODE_SPI_RX_OVERRUN = 114,

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
  OBC_ERR_CODE_INVALID_AX25_PACKET,
  OBC_ERR_CODE_CC1120_RECEIVE_FAILURE,
  OBC_ERR_CODE_CC1120_RECEIVE_TERMINATED,
  OBC_ERR_CODE_FEC_ENCODE_FAILURE,
  OBC_ERR_CODE_FEC_DECODE_FAILURE,
  OBC_ERR_CODE_AX25_ENCODE_FAILURE,
  OBC_ERR_CODE_AX25_DECODE_FAILURE,
  OBC_ERR_CODE_AX25_BIT_STUFF_FAILURE,
  OBC_ERR_CODE_AX25_BIT_UNSTUFF_FAILURE,
  OBC_ERR_CODE_AES_DECRYPT_FAILURE,
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

  OBC_ERR_CODE_VN100_RESPONSE_ERROR = 901,
  OBC_ERR_CODE_VN100_PARSE_ERROR = 902,
  /* Reliance edge errors */
  // Values are mapped as (ORIGINAL_RED_ERRCODE + RELIANCE_EDGE_ERROR_CODES_OFFSET)
  /** Operation not permitted. */
  OBC_ERR_CODE_RED_EPERM = 1001,
  OBC_ERR_CODE_RED_ENOENT = 1002,

  /** I/O error. */
  OBC_ERR_CODE_RED_EIO = 1005,

  /** Bad file number. */
  OBC_ERR_CODE_RED_EBADF = 1009,

  /** Out of memory */
  OBC_ERR_CODE_RED_ENOMEM = 1012,

  /** Permission denied. */
  OBC_ERR_CODE_RED_EACCES = 1013,

  /** Device or resource busy. */
  OBC_ERR_CODE_RED_EBUSY = 1016,

  /** File exists. */
  OBC_ERR_CODE_RED_EEXIST = 1017,

  /** Cross-device link. */
  OBC_ERR_CODE_RED_EXDEV = 1018,

  /** Not a directory. */
  OBC_ERR_CODE_RED_ENOTDIR = 1020,

  /** Is a directory. */
  OBC_ERR_CODE_RED_EISDIR = 1021,

  /** Invalid argument. */
  OBC_ERR_CODE_RED_EINVAL = 1022,

  /** File table overflow. */
  OBC_ERR_CODE_RED_ENFILE = 1023,

  /** Too many open files. */
  OBC_ERR_CODE_RED_EMFILE = 1024,

  /** File too large. */
  OBC_ERR_CODE_RED_EFBIG = 1027,

  /** No space left on device. */
  OBC_ERR_CODE_RED_ENOSPC = 1028,

  /** Read-only file system. */
  OBC_ERR_CODE_RED_EROFS = 1030,

  /** Too many links. */
  OBC_ERR_CODE_RED_EMLINK = 1031,

  /** Math result not representable. */
  OBC_ERR_CODE_RED_ERANGE = 1034,

  /** File name too long. */
  OBC_ERR_CODE_RED_ENAMETOOLONG = 1036,

  /** Function not implemented. */
  OBC_ERR_CODE_RED_ENOSYS = 1038,

  /** Directory not empty. */
  OBC_ERR_CODE_RED_ENOTEMPTY = 1039,

  /** Too many symbolic links encountered. */
  OBC_ERR_CODE_RED_ELOOP = 1040,

  /** No data available. */
  OBC_ERR_CODE_RED_ENODATA = 1061,

  /** Link has been severed. */
  OBC_ERR_CODE_RED_ENOLINK = 1067,

  /** Too many users. */
  OBC_ERR_CODE_RED_EUSERS = 1087,

  /** Operation is not supported. */
  OBC_ERR_CODE_RED_ENOTSUPP = 1524,

} obc_error_code_t;
