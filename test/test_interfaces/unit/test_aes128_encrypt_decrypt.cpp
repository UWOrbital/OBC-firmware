#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <gtest/gtest.h>

TEST(TestObcGsAes128, testingFailConditions) {
  // testing null value for initializeAESCtx
  const uint8_t key = 0;
  obc_gs_error_code_t errCode = initializeAesCtx(NULL);
  EXPECT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

  // testing null values for aes128decrypt and aes128Encrypt
  aes_data_t test1;
  uint8_t inputArray[AES_BLOCK_SIZE];
  uint8_t outputBufferLen = AES_BLOCK_SIZE;

  errCode = aes128Decrypt(NULL, inputArray, outputBufferLen);
  EXPECT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

  errCode = aes128Decrypt(&test1, NULL, outputBufferLen);
  EXPECT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

  uint8_t outputBufferLenGreaterThan = 17U;  // 1U greater than AES_BLOCK_SIZE
  errCode = aes128Decrypt(&test1, inputArray, outputBufferLenGreaterThan);
  EXPECT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);
  /*
      test1.ciphertext = NULL;
      errCode = aes128Decrypt(&test1, inputArray, outputBufferLen);
      EXPECT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG); */
}

TEST(TestObcGsAes128, testingEncryptDecryptSameMsg) {
  // initalize ctx
  const uint8_t key[AES_BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                       0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx(key);

  // setting up variables
  aes_data_t test1;
  uint8_t BufferLen = AES_BLOCK_SIZE;
  uint8_t message[AES_BLOCK_SIZE] = {0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
                                     0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20};
  uint8_t encryptedData[AES_BLOCK_SIZE];
  uint8_t initializationVector[AES_IV_SIZE] = {0xC1, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
  memcpy(test1.iv, initializationVector, sizeof(initializationVector));
  test1.textLen = BufferLen;
  test1.ciphertext = &message[0];
  // testing

  aes128Encrypt(&test1, encryptedData, BufferLen);

  test1.ciphertext = &encryptedData[0];

  aes128Decrypt(&test1, encryptedData, BufferLen);

  int messageAndAESProcessEqual = memcmp(message, encryptedData, sizeof(encryptedData));
  ASSERT_EQ(messageAndAESProcessEqual, 0);
}

/* The idea for this test is to imitate a "stream" of data that needs to be encrypted all
   together then decrypted all together. Used three seperate messages to make it more "clear" instead
   of one big block that needs to be iterated over.
*/
TEST(TestObcGsAes128, testingEncryptDecryptMultipleBlocks) {
  // initalize ctx
  const uint8_t key[AES_BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                       0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  obc_gs_error_code_t initializeCheck = initializeAesCtx(key);
  ASSERT_EQ(initializeCheck, OBC_GS_ERR_CODE_SUCCESS);

  // setting up variables
  aes_data_t test1;
  aes_data_t test2;
  aes_data_t test3;

  uint8_t BufferLen = AES_BLOCK_SIZE;

  uint8_t message1[AES_BLOCK_SIZE] = {0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
                                      0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20};

  uint8_t message2[AES_BLOCK_SIZE] = {0x1,  0x3,  0x5,  0x7,  0x9,  0xB,  0xD,  0xF,
                                      0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F};

  uint8_t message3[AES_BLOCK_SIZE] = {0x2,  0x3,  0x5,  0x7,  0xB,  0xD,  0x11, 0x13,
                                      0x17, 0x1D, 0x1F, 0x25, 0x29, 0x2B, 0x2F, 0x35};

  uint8_t encryptedMessage1[AES_BLOCK_SIZE];
  uint8_t encryptedMessage2[AES_BLOCK_SIZE];
  uint8_t encryptedMessage3[AES_BLOCK_SIZE];

  uint8_t encryptedData[AES_BLOCK_SIZE];

  uint8_t initializationVector[AES_IV_SIZE] = {0xC1, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

  memcpy(test1.iv, initializationVector, sizeof(initializationVector));
  memcpy(test2.iv, initializationVector, sizeof(initializationVector));
  memcpy(test3.iv, initializationVector, sizeof(initializationVector));

  test1.textLen = test2.textLen = test3.textLen = BufferLen;
  test1.ciphertext = &message1[0];
  test2.ciphertext = &message2[0];
  test3.ciphertext = &message3[0];

  // testing
  aes128Encrypt(&test1, encryptedMessage1, BufferLen);
  aes128Encrypt(&test2, encryptedMessage2, BufferLen);
  aes128Encrypt(&test3, encryptedMessage3, BufferLen);

  test1.ciphertext = &encryptedMessage1[0];
  test2.ciphertext = &encryptedMessage2[0];
  test3.ciphertext = &encryptedMessage3[0];

  aes128Decrypt(&test1, encryptedMessage1, BufferLen);
  aes128Decrypt(&test2, encryptedMessage2, BufferLen);
  aes128Decrypt(&test3, encryptedMessage3, BufferLen);

  int ensureMessageQuality1 = memcmp(encryptedMessage1, message1, sizeof(message1));
  int ensureMessageQuality2 = memcmp(encryptedMessage2, message2, sizeof(message2));
  int ensureMessageQuality3 = memcmp(encryptedMessage3, message3, sizeof(message3));

  EXPECT_EQ(ensureMessageQuality1, 0);
  EXPECT_EQ(ensureMessageQuality2, 0);
  EXPECT_EQ(ensureMessageQuality3, 0);
}
