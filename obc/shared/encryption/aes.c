// ─── Imports ────────────────────────────────────────────────────
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_LEN 16  // 16 bytes -> 128 bits (1 byte => 8 bits)

/**
 * Reference for AES-128: https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf
 */
// ─── Transformations/Operations ─────────────────────────────────

void subBytes() { return; }

void shiftRows() { return; }

void mixColumns() { return; }

void addRoundKey() { return; }

void keyExpansion() { return; }

void invShiftRows() { return; }

void invSubBytes() { return; }

void invMixColumns() { return; }

// Either truncates or pads a string to match 128 bits/16 bytes
void sanitizeKey(const char* input, unsigned char out[KEY_LEN]) {
  size_t len = strlen(input);
  for (int i = 0; i < KEY_LEN; ++i) {
    if (i < (int)len) {
      out[i] = (unsigned char)input[i];
    } else {
      out[i] = 0x00;
    }
  }
}

// ─── User function ──────────────────────────────────────────────
void encrypt() { return; }

void decrypt() { return; }

// Used for testing during development
int main() {
  char secret[] = "my-secret-key";
  unsigned char key[KEY_LEN];
  sanitizeKey(secret, key);

  for (int i = 0; i < KEY_LEN; ++i) {
    printf("%02X ", key[i]);
  }
  printf("\n");

  return 0;
}