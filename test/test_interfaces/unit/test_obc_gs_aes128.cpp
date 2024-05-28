#include "obc_gs_aes128.h"

#include <stdint.h>
#include <string.h>

#include <gtest/gtest.h>

TEST(TestAes128, TestAes128EncryptDecrypt) {
  const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  uint8_t ciphertext[AES_BLOCK_SIZE] = {0};
  aes_data_t aesData = {.iv = {0}, .ciphertext = ciphertext, .ciphertextLen = AES_BLOCK_SIZE};
  aesData.ciphertextLen = AES_BLOCK_SIZE;
  memcpy(aesData.iv, TEMP_STATIC_KEY, AES_IV_SIZE);

  uint8_t input[AES_BLOCK_SIZE] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
                                   0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
  uint8_t inputBufferLen = AES_BLOCK_SIZE;

  uint8_t output[AES_BLOCK_SIZE];
  uint8_t outputBufferLen = AES_BLOCK_SIZE;

  ASSERT_EQ(initializeAesCtx(TEMP_STATIC_KEY), OBC_GS_ERR_CODE_SUCCESS);

  ASSERT_EQ(aes128Encrypt(input, inputBufferLen, &aesData), OBC_GS_ERR_CODE_SUCCESS);
  ASSERT_EQ(aes128Decrypt(&aesData, output, outputBufferLen), OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(memcmp(input, output, AES_BLOCK_SIZE), 0);
}
