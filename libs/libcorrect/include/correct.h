#ifndef CORRECT_H
#define CORRECT_H
#include <stdint.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <stddef.h>
typedef ptrdiff_t ssize_t;
#endif

// Reed-Solomon

struct correct_reed_solomon;
typedef struct correct_reed_solomon correct_reed_solomon;

static const uint16_t correct_rs_primitive_polynomial_8_4_3_2_0 =
    0x11d;  // x^8 + x^4 + x^3 + x^2 + 1

static const uint16_t correct_rs_primitive_polynomial_8_5_3_1_0 =
    0x12b;  // x^8 + x^5 + x^3 + x + 1

static const uint16_t correct_rs_primitive_polynomial_8_5_3_2_0 =
    0x12d;  // x^8 + x^5 + x^3 + x^2 + 1

static const uint16_t correct_rs_primitive_polynomial_8_6_3_2_0 =
    0x14d;  // x^8 + x^6 + x^3 + x^2 + 1

static const uint16_t correct_rs_primitive_polynomial_8_6_4_3_2_1_0 =
    0x15f;  // x^8 + x^6 + x^4 + x^3 + x^2 + x + 1;

static const uint16_t correct_rs_primitive_polynomial_8_6_5_1_0 =
    0x163;  // x^8 + x^6 + x^5 + x + 1

static const uint16_t correct_rs_primitive_polynomial_8_6_5_2_0 =
    0x165;  // x^8 + x^6 + x^5 + x^2 + 1

static const uint16_t correct_rs_primitive_polynomial_8_6_5_3_0 =
    0x169;  // x^8 + x^6 + x^5 + x^3 + 1

static const uint16_t correct_rs_primitive_polynomial_8_6_5_4_0 =
    0x171;  // x^8 + x^6 + x^5 + x^4 + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_2_1_0 =
    0x187;  // x^8 + x^7 + x^2 + x + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_3_2_0 =
    0x18d;  // x^8 + x^7 + x^3 + x^2 + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_5_3_0 =
    0x1a9;  // x^8 + x^7 + x^5 + x^3 + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_6_1_0 =
    0x1c3;  // x^8 + x^7 + x^6 + x + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_6_3_2_1_0 =
    0x1cf;  // x^8 + x^7 + x^6 + x^3 + x^2 + x + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_6_5_2_1_0 =
    0x1e7;  // x^8 + x^7 + x^6 + x^5 + x^2 + x + 1

static const uint16_t correct_rs_primitive_polynomial_8_7_6_5_4_2_0 =
    0x1f5;  // x^8 + x^7 + x^6 + x^5 + x^4 + x^2 + 1

static const uint16_t correct_rs_primitive_polynomial_ccsds =
    0x187;  // x^8 + x^7 + x^2 + x + 1

/* correct_reed_solomon_create allocates and initializes an
 * encoder/decoder for a given reed solomon error correction
 * code. The block size must be 255 bytes with 8-bit symbols.
 *
 * This block can repair corrupted bytes. It can handle as
 * many as num_roots/2 bytes having corruption and still recover
 * the encoded payload. However, using more num_roots
 * adds more parity overhead and substantially increases
 * the computational time for decoding.
 *
 * primitive_polynomial should be one of the given values in this
 * file. Sane values for first_consecutive_root and
 * generator_root_gap are 1 and 1. Not all combinations of
 * values produce valid codes.
 */
correct_reed_solomon *correct_reed_solomon_create(uint16_t primitive_polynomial,
                                                  uint8_t first_consecutive_root,
                                                  uint8_t generator_root_gap,
                                                  size_t num_roots);

/* correct_reed_solomon_encode uses the rs instance to encode
 * parity information onto a block of data. msg_length should be
 * no more than the payload size for one block e.g. no more
 * than 223 for a (255, 223) code. Shorter blocks will be encoded
 * with virtual padding where the padding is not emitted.
 *
 * encoded should be at least msg_length + parity length bytes long
 *
 * It is allowable for msg and encoded to be the same pointer. In
 * that case, the parity bytes will be written after the msg bytes
 * end.
 *
 * This function returns the number of bytes written to encoded.
 */
ssize_t correct_reed_solomon_encode(correct_reed_solomon *rs, const uint8_t *msg, size_t msg_length,
                                    uint8_t *encoded);

/* correct_reed_solomon_decode uses the rs instance to decode
 * a payload from a block containing payload and parity bytes.
 * This function can recover in spite of some bytes being corrupted.
 *
 * In most cases, if the block is too corrupted, this function
 * will return -1 and not perform decoding. It is possible but
 * unlikely that the payload written to msg will contain
 * errors when this function returns a positive value.
 *
 * msg should be long enough to contain a decoded payload for
 * this encoded block.
 *
 * This function returns a positive number of bytes written to msg
 * if it has decoded or -1 if it has encountered an error.
 */
ssize_t correct_reed_solomon_decode(correct_reed_solomon *rs, const uint8_t *encoded,
                                    size_t encoded_length, uint8_t *msg);

/* correct_reed_solomon_decode_with_erasures uses the rs
 * instance to decode a payload from a block containing payload
 * and parity bytes. Additionally, the user can provide the
 * indices of bytes which have been suspected to be corrupted.
 * This erasure information is typically provided by a demodulating
 * or receiving device. This function can recover with
 * some additional errors on top of the erasures.
 *
 * In order to successfully decode, the quantity
 * (num_erasures + 2*num_errors) must be less than
 * num_roots.
 *
 * erasure_locations shold contain erasure_length items.
 * erasure_length should not exceed the number of parity
 * bytes encoded into this block.
 *
 * In most cases, if the block is too corrupted, this function
 * will return -1 and not perform decoding. It is possible but
 * unlikely that the payload written to msg will contain
 * errors when this function returns a positive value.
 *
 * msg should be long enough to contain a decoded payload for
 * this encoded block.
 *
 * This function returns a positive number of bytes written to msg
 * if it has decoded or -1 if it has encountered an error.
 */
ssize_t correct_reed_solomon_decode_with_erasures(correct_reed_solomon *rs, const uint8_t *encoded,
                                                  size_t encoded_length,
                                                  const uint8_t *erasure_locations,
                                                  size_t erasure_length, uint8_t *msg);

/* correct_reed_solomon_destroy releases the resources
 * associated with rs. This pointer should not be
 * used for any functions after this call.
 */
void correct_reed_solomon_destroy(correct_reed_solomon *rs);

#endif

