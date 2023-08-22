#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"


#include <gtest/gtest.h>

TEST(AES128, testingFailConditions) {
    // testing null value for initializeAESCtx
    const uint8_t key = 0;
    obc_gs_error_code_t errCode = initializeAesCtx(NULL);
    ASSERT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

    // testing null values for aes128Encrypt
    aes_data_t test1;
    uint8_t inputArray[AES_BLOCK_SIZE];
    uint8_t outputBufferLen = AES_BLOCK_SIZE;

    errCode = aes128Decrypt(NULL, &inputArray[0], outputBufferLen);
    ASSERT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

    errCode = aes128Decrypt(&test1, NULL, outputBufferLen);
    ASSERT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

    uint8_t outputBufferLenGreaterThan = 17U; // 1U greater than AES_BLOCK_SIZE
    errCode = aes128Decrypt(&test1, &inputArray[0], outputBufferLenGreaterThan);
    ASSERT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);
}

TEST(AES128, testingInitialiseAESSuccess) {
    const uint8_t key[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    obc_gs_error_code_t errCode = initializeAesCtx(&key[0]);
    ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);
}

TEST(AES128, testingEncryptDecryptSameMsg) {
    // initalize ctx
    const uint8_t key[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    initializeAesCtx(&key[0]);
    
    // setting up variables
    aes_data_t test1;
    uint8_t outputBufferLen = AES_BLOCK_SIZE;
    uint8_t message[16] = {
        0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
        0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20};
    uint8_t encryptedData[16];
    uint8_t initializationVector[16] = {
        0xC1, 0x23, 0x45, 0x67,
        0x89, 0xAB, 0xCD, 0xEF,
        0x01, 0x23, 0x45, 0x67,
        0x89, 0xAB, 0xCD, 0xEF};
    memcpy(test1.iv, initializationVector, sizeof(initializationVector));
    test1.textLen = outputBufferLen;
    test1.xcryptText = &message[0];
    // testing
    
    aes128Encrypt(&test1, encryptedData, outputBufferLen);
    
    test1.xcryptText = &encryptedData[0];
    
    aes128Decrypt(&test1, encryptedData, outputBufferLen);
    
    int messageAndAESProcessEqual = memcmp(message, encryptedData, sizeof(encryptedData));
    ASSERT_EQ(messageAndAESProcessEqual, 0);
}