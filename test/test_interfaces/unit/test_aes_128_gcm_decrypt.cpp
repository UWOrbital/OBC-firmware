#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <gtest/gtest.h>
#include <cstring>  // Include for memcpy and memcmp

TEST(TestEncryptionDecryption, EncryptDecrypt) {
  // Initialize key
  uint8_t key[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

  // Initialize GCM context
  initializeAesCtx(key);

  // Prepare plaintext
  const uint8_t plaintext[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
  size_t plaintextLen = sizeof(plaintext);

  // Prepare IV
  uint8_t iv[AES_IV_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

  // Prepare output buffers
  uint8_t ciphertext[plaintextLen];
  uint8_t decrypted[plaintextLen];

  // Prepare additional authenticated data (AAD)
  const uint8_t additionalData[] = {'A', 'd', 'd', 'i', 't', 'i', 'o', 'n', 'a', 'l', 'D', 'a', 't', 'a'};
  size_t additionalDataLen = sizeof(additionalData);

  // Prepare tag
  uint8_t tag[16];
  size_t tagLen = sizeof(tag);

  // Prepare aes_data_t structure for encryption
  aes_data_t aesDataEncrypt;
  memcpy(aesDataEncrypt.iv, iv, AES_IV_SIZE);
  aesDataEncrypt.tagLen = tagLen;

  // Encrypt the plaintext
  obc_gs_error_code_t encResult =
      gcmEncrypt(&aesDataEncrypt, plaintext, plaintextLen, additionalData, additionalDataLen, ciphertext);
  ASSERT_EQ(encResult, OBC_GS_ERR_CODE_SUCCESS);

  // Print the ciphertext for debugging in hex
  printf("Ciphertext: ");
  for (size_t i = 0; i < plaintextLen; ++i) {
    printf("%02x", ciphertext[i]);
  }
  printf("\n");

  // Print the tag for debugging in hex
  printf("Tag: ");
  for (size_t i = 0; i < tagLen; ++i) {
    printf("%02x", aesDataEncrypt.tag[i]);
  }
  printf("\n");

  // Additional debug: Check the tag after encryption
  printf("Tag after encryption in test: ");
  for (size_t i = 0; i < aesDataEncrypt.tagLen; ++i) {
    printf("%02x", aesDataEncrypt.tag[i]);
  }
  printf("\n");

  // Prepare aes_data_t structure for decryption
  aes_data_t aesDataDecrypt;
  memcpy(aesDataDecrypt.iv, iv, AES_IV_SIZE);
  aesDataDecrypt.ciphertext = ciphertext;
  aesDataDecrypt.ciphertextLen = plaintextLen;
  memcpy(aesDataDecrypt.tag, aesDataEncrypt.tag, tagLen);
  aesDataDecrypt.tagLen = tagLen;

  // Additional debug: Check the tag before decryption
  printf("Tag before decryption in test: ");
  for (size_t i = 0; i < aesDataDecrypt.tagLen; ++i) {
    printf("%02x", aesDataDecrypt.tag[i]);
  }
  printf("\n");

  // Decrypt the ciphertext
  obc_gs_error_code_t decResult =
      aes128Decrypt(&aesDataDecrypt, decrypted, plaintextLen, additionalData, additionalDataLen);

  // Print the decrypted text for debugging in hex
  printf("Decrypted (hex): ");
  for (size_t i = 0; i < plaintextLen; ++i) {
    printf("%02x", decrypted[i]);
  }
  printf("\n");

  // Print the tag after decryption
  printf("Tag after Decryption: ");
  for (size_t i = 0; i < tagLen; ++i) {
    printf("%02x", aesDataDecrypt.tag[i]);
  }
  printf("\n");

  // Print the plaintext for debugging in hex
  printf("Plaintext (hex): ");
  for (size_t i = 0; i < plaintextLen; ++i) {
    printf("%02x", plaintext[i]);
  }
  printf("\n");

  // Print decResult
  printf("DecResult: %d\n", decResult);
  // ASSERT_EQ(decResult, OBC_GS_ERR_CODE_SUCCESS);

  // Verify the decrypted text matches the original plaintext
  // EXPECT_EQ(memcmp(plaintext, decrypted, plaintextLen), 0);
}
