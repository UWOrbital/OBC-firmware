#ifndef MBEDTLS_CONFIG_H
// Build mbtedtls with this as your configuration file and you'll have only what you need
// for AES GCM. You'll find a pretty small statically linked exe, at least when compared
// with WolfSL, OpenSSL, etc, etc. which (to be fair) are really meant to always have some
// amount of SSL/TLS support enabled
// Enable platform memory functions
// #define MBEDTLS_PLATFORM_MEMORY

// // Enable platform support
// #define MBEDTLS_PLATFORM_C

// // Enable platform memory functions
// #define MBEDTLS_PLATFORM_MEMORY

// // Enable the buffer allocator
// #define MBEDTLS_MEMORY_BUFFER_ALLOC_C


#define MBEDTLS_CONFIG_H
#define MBEDTLS_AES_FEWER_TABLES
#define MBEDTLS_NO_UDBL_DIVISION
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_GCM_C
#define MBEDTLS_CCM_GCM_CAN_AES
#define MBEDTLS_CONFIG_IS_FINALIZED

#define MBEDTLS_CCM_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CFB
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CIPHER_MODE_OFB
#define MBEDTLS_CIPHER_MODE_XTS




#include "check_config.h"
#endif /* MBEDTLS_CONFIG_H */
