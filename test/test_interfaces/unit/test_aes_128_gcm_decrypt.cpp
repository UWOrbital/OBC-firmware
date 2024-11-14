#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <gtest/gtest.h>
#include <cstring>  // Include for memcpy and memcmp

TEST(TestEncryptionDecryption, EncryptDecrypt) {
  // Initialize key
  uint8_t key[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

  // Initialize GCM context
  obc_gs_error_code_t initResult = initializeAesCtx(key);
  ASSERT_EQ(initResult, OBC_GS_ERR_CODE_SUCCESS);

  // Prepare plaintext
  const char *plaintext = "Hello, World!";
  size_t plaintextLen = strlen(plaintext);

  // Prepare aes_data_t structure
  aes_data_t aesData = {0};  // Initialize all fields to zero

  // Prepare IV
  uint8_t iv[AES_IV_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  memcpy(aesData.iv, iv, AES_IV_SIZE);

  // Prepare ciphertext buffer
  uint8_t ciphertext[plaintextLen];
  aesData.ciphertext = ciphertext;
  aesData.ciphertextLen = plaintextLen;

  // Prepare decrypted output buffer
  uint8_t decryptedOutput[plaintextLen];

  // Prepare additional authenticated data (AAD)
  const char *additionalData = "AdditionalData";
  size_t additionalDataLen = strlen(additionalData);
  aesData.additionalData = const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(additionalData));
  aesData.additionalDataLen = additionalDataLen;

  // Set the tag length (but don't set the tag contents)
  aesData.tagLen = AES_TAG_SIZE;

  // Encrypt the plaintext
  obc_gs_error_code_t encResult = aes128Encrypt(&aesData, reinterpret_cast<const uint8_t *>(plaintext), plaintextLen);
  ASSERT_EQ(encResult, OBC_GS_ERR_CODE_SUCCESS);

  // The tag has now been filled by the encryption function

  // Decrypt the ciphertext
  obc_gs_error_code_t decResult = aes128Decrypt(&aesData, decryptedOutput, plaintextLen);
  ASSERT_EQ(decResult, OBC_GS_ERR_CODE_SUCCESS);

  // Verify the decrypted text matches the original plaintext
  EXPECT_EQ(memcmp(plaintext, decryptedOutput, plaintextLen), 0);
}

TEST(TestEncryptionDecryption, DecryptWithInvalidTag) {
  // Initialize key
  uint8_t key[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

  // Initialize GCM context
  obc_gs_error_code_t initResult = initializeAesCtx(key);
  ASSERT_EQ(initResult, OBC_GS_ERR_CODE_SUCCESS);

  // Prepare plaintext
  const char *plaintext = "Hello, World!";
  size_t plaintextLen = strlen(plaintext);

  // Prepare aes_data_t structure
  aes_data_t aesData = {0};  // Initialize all fields to zero

  // Prepare IV
  uint8_t iv[AES_IV_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  memcpy(aesData.iv, iv, AES_IV_SIZE);

  // Prepare ciphertext buffer
  uint8_t ciphertext[plaintextLen];
  aesData.ciphertext = ciphertext;
  aesData.ciphertextLen = plaintextLen;

  // Prepare decrypted output buffer
  uint8_t decryptedOutput[plaintextLen];

  // Prepare additional authenticated data (AAD)
  const char *additionalData = "AdditionalData";
  size_t additionalDataLen = strlen(additionalData);
  aesData.additionalData = const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(additionalData));
  aesData.additionalDataLen = additionalDataLen;

  // Set the tag length (but don't set the tag contents)
  aesData.tagLen = AES_TAG_SIZE;

  // Encrypt the plaintext
  obc_gs_error_code_t encResult = aes128Encrypt(&aesData, reinterpret_cast<const uint8_t *>(plaintext), plaintextLen);
  ASSERT_EQ(encResult, OBC_GS_ERR_CODE_SUCCESS);

  // The tag has now been filled by the encryption function

  // Modify the tag to make it invalid
  aesData.tag[0] ^= 0xFF;  // Flip all bits in the first byte of the tag

  // Attempt to decrypt with invalid tag
  obc_gs_error_code_t decResult = aes128Decrypt(&aesData, decryptedOutput, plaintextLen);

  // Check that the decryption failed due to authentication failure
  ASSERT_EQ(decResult, OBC_GS_ERR_CODE_AUTH_FAILED);
}
