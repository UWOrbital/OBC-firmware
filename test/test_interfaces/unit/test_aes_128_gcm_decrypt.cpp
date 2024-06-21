#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <gtest/gtest.h>
#include <string.h>

TEST(TestObcGsAes128, SingleBlockEncryptDecrypt) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx(key);

  aes_data_t aesData;
  uint8_t plaintext[16] = {0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
                           0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
  uint8_t ciphertext[16];
  uint8_t decryptedtext[16];
  uint8_t iv[12] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};
  uint8_t tag[16];

  aesData.iv = iv;
  aesData.ciphertext = ciphertext;
  aesData.ciphertextLen = sizeof(ciphertext);

  // Encrypt

  // Decrypt
  obc_gs_error_code_t result = aes128Decrypt(&aesData, decryptedtext, sizeof(decryptedtext), tag, sizeof(tag));
  ASSERT_EQ(result, OBC_GS_ERR_CODE_SUCCESS);
  ASSERT_EQ(memcmp(plaintext, decryptedtext, sizeof(plaintext)), 0);
}

TEST(TestObcGsAes128, MultipleBlocksEncryptDecrypt) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx(key);

  uint8_t plaintext[48] = {0};
  uint8_t ciphertext[48];
  uint8_t decryptedtext[48];
  uint8_t iv[12] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};
  uint8_t tag[16];
  aes_data_t aesData;

  aesData.iv = iv;
  aesData.ciphertext = ciphertext;
  aesData.ciphertextLen = sizeof(ciphertext);

  // Encrypt

  // Decrypt
  obc_gs_error_code_t result = aes128Decrypt(&aesData, decryptedtext, sizeof(decryptedtext), tag, sizeof(tag));
  ASSERT_EQ(result, OBC_GS_ERR_CODE_SUCCESS);
  ASSERT_EQ(memcmp(plaintext, decryptedtext, sizeof(plaintext)), 0);
}
