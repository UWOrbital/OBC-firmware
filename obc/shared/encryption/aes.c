// ─── Imports ────────────────────────────────────────────────────
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define KEY_LEN 16  // 16 bytes -> 128 bits (1 byte => 8 bits)
#define Nb 4
#define Nk 4
#define Nr 10

const uint32_t Rcon[Nr] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
                           0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

static const uint8_t sbox[16][16] = {
    /*         y=0    1     2     3     4     5     6     7     8     9     a     b     c     d     e     f  */
    /* x=0 */ {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
    /* x=1 */ {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
    /* x=2 */ {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
    /* x=3 */ {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
    /* x=4 */ {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
    /* x=5 */ {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
    /* x=6 */ {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
    /* x=7 */ {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
    /* x=8 */ {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
    /* x=9 */ {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
    /* x=a */ {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
    /* x=b */ {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
    /* x=c */ {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
    /* x=d */ {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
    /* x=e */ {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
    /* x=f */ {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}};

/**
 * Reference for AES-128: https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf
 */
// ─── Transformations/Operations ─────────────────────────────────

void subBytes() { return; }

void shiftRows() { return; }

void mixColumns() { return; }

void addRoundKey(uint8_t state[4][4], const uint32_t rk[4]) {
  for (size_t c = 0; c < 4; ++c) {
    uint32_t col =
        (uint32_t)state[0][c] << 24 | (uint32_t)state[1][c] << 16 | (uint32_t)state[2][c] << 8 | (uint32_t)state[3][c];

    col ^= rk[c];
    state[0][c] = (uint8_t)(col >> 24);
    state[1][c] = (uint8_t)(col >> 16);
    state[2][c] = (uint8_t)(col >> 8);
    state[3][c] = (uint8_t)(col & 0xFF);
  }

  return;
}

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

uint32_t RotWord(const uint32_t word) { return (word << 8) | (word >> 24); }

uint32_t SubWord(const uint32_t word) {
  uint8_t sub[4] = {(uint8_t)(word >> 24), (uint8_t)(word >> 16), (uint8_t)(word >> 8), (uint8_t)word};
  sub[0] = sbox[sub[0] >> 4][sub[0] & 0x0F];
  sub[1] = sbox[sub[1] >> 4][sub[1] & 0x0F];
  sub[2] = sbox[sub[2] >> 4][sub[2] & 0x0F];
  sub[3] = sbox[sub[3] >> 4][sub[3] & 0x0F];

  return ((uint32_t)sub[0] << 24) | ((uint32_t)sub[1] << 16) | ((uint32_t)sub[2] << 8) | (uint32_t)sub[3];
}

void keyExpansion(const unsigned char* key, uint32_t w[44]) {
  for (size_t i = 0; i < (4 * Nr) + 4; ++i) {
    if (i < Nk) {
      w[i] = (uint32_t)key[4 * i] << 24 | (uint32_t)key[4 * i + 1] << 16 | (uint32_t)key[4 * i + 2] << 8 |
             (uint32_t)key[4 * i + 3];
    } else {
      uint32_t temp = w[i - 1];
      if (i % Nk == 0) {
        temp = SubWord(RotWord(temp)) ^ Rcon[(i / Nk) - 1];
      }
      w[i] = w[i - Nk] ^ temp;
    }
  }

  return;
}

// ─── User function ──────────────────────────────────────────────
void encrypt(const char input[], const unsigned char* key) {
  printf("Plaintext: %s\n", input);

  // Array we do our operations on. Map flat array into 2D array
  // State is now state[row][col]
  uint8_t state[4][4];
  printf("State: ");
  for (size_t col = 0; col < 4; ++col) {
    for (size_t row = 0; row < 4; ++row) {
      state[row][col] = (uint8_t)input[row + 4 * col];
      printf("%02X\n", state[row][col]);
    }
  }
  printf("\n");

  // Generate the round keys
  uint32_t w[44];
  keyExpansion(key, w);

  // Round 0:
  uint32_t rk[4] = {w[0], w[1], w[2], w[3]};
  addRoundKey(state, rk);

  for (size_t round = 1; round < (Nr - 1); ++round) {
    // subBytes(state);
  }

  return;
}

void decrypt() { return; }

static int g_run = 0, g_pass = 0;

static void check_word(const char* name, uint32_t got, uint32_t expected) {
  g_run++;
  if (got == expected) {
    g_pass++;
    printf("[PASS] %-28s = 0x%08X\n", name, (unsigned)got);
  } else {
    printf("[FAIL] %-28s : got 0x%08X, expected 0x%08X\n", name, (unsigned)got, (unsigned)expected);
  }
}

static void check_state(const char* name, const uint8_t in[16], const uint32_t rk[4], const uint8_t expected[16]) {
  g_run++;
  uint8_t state[4][4];
  for (size_t i = 0; i < 16; ++i) state[i % 4][i / 4] = in[i];  // column-major load: state[row][col]=in[row+4col]
  addRoundKey(state, rk);
  uint8_t out[16];
  for (size_t i = 0; i < 16; ++i) out[i] = state[i % 4][i / 4];  // inverse serialize
  int ok = 1;
  for (size_t i = 0; i < 16; ++i)
    if (out[i] != expected[i]) ok = 0;
  if (ok) {
    g_pass++;
    printf("[PASS] %-34s\n", name);
  } else {
    printf("[FAIL] %-34s\n       got     :", name);
    for (size_t i = 0; i < 16; ++i) printf(" %02X", out[i]);
    printf("\n       expected:");
    for (size_t i = 0; i < 16; ++i) printf(" %02X", expected[i]);
    printf("\n");
  }
}

// Used for testing during development
int main() {
  char secret[] = "my-secret-key";
  unsigned char key[KEY_LEN];
  sanitizeKey(secret, key);

  for (int i = 0; i < KEY_LEN; ++i) {
    printf("%02X ", key[i]);
  }
  printf("\n");

  char plainText[] = "plaintext tests";
  encrypt(plainText, key);

  // ── RotWord: [a0,a1,a2,a3] -> [a1,a2,a3,a0] ──
  check_word("RotWord(0x09cf4f3c)", RotWord(0x09cf4f3c), 0xcf4f3c09);
  check_word("RotWord(0x01020304)", RotWord(0x01020304), 0x02030401);
  check_word("RotWord(0x00000000)", RotWord(0x00000000), 0x00000000);
  check_word("RotWord(0xff000000)", RotWord(0xff000000), 0x000000ff);

  // ── SubWord: S-box applied to each of the 4 bytes ──
  check_word("SubWord(0x00000000)", SubWord(0x00000000), 0x63636363);
  check_word("SubWord(0xffffffff)", SubWord(0xffffffff), 0x16161616);
  check_word("SubWord(0x01234567)", SubWord(0x01234567), 0x7c266e85);
  check_word("SubWord(0xcf4f3c09)", SubWord(0xcf4f3c09), 0x8a84eb01);

  // ── Composition: matches the w[4] derivation for the FIPS test key ──
  check_word("SubWord(RotWord(0x09cf4f3c))", SubWord(RotWord(0x09cf4f3c)), 0x8a84eb01);
  // ── addRoundKey ──
  {
    // A: all-zero round key leaves the state unchanged  (b ^ 0 = b)
    uint8_t in_a[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint32_t rk_a[4] = {0, 0, 0, 0};
    uint8_t exp_a[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    check_state("addRoundKey zero key = identity", in_a, rk_a, exp_a);

    // B: XOR a state with its own column words zeroes it  (b ^ b = 0)
    uint8_t in_b[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint32_t rk_b[4] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f};
    uint8_t exp_b[16] = {0};  // all zero
    check_state("addRoundKey self-cancel = zero", in_b, rk_b, exp_b);

    // C: FIPS-197 AES-128 example, initial AddRoundKey (round 0)
    uint8_t in_c[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint32_t rk_c[4] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f};
    uint8_t exp_c[16] = {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                         0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0};
    check_state("addRoundKey FIPS round-0 vector", in_c, rk_c, exp_c);

    // D: zero state -> output is the round-key bytes, placed column-by-column
    uint8_t in_d[16] = {0};
    uint32_t rk_d[4] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff};
    uint8_t exp_d[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    check_state("addRoundKey places rk[c] in column c", in_d, rk_d, exp_d);
  }

  printf("\n%d/%d tests passed\n", g_pass, g_run);

  return 0;
}