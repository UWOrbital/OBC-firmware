#include "rs_tester.h"
#include "sys_heap.h"


void shuffle(int *a, size_t len) {
    for (size_t i = 0; i < len - 2; i++) {
        size_t j = rand() % (len - i) + i;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
}

void rs_correct_encode(void *encoder, uint8_t *msg, size_t msg_length,
                       uint8_t *msg_out) {
    correct_reed_solomon_encode((correct_reed_solomon *)encoder, msg,
                                msg_length, msg_out);
}

void rs_correct_decode(void *decoder, uint8_t *encoded, size_t encoded_length,
                       uint8_t *erasure_locations, size_t erasure_length,
                       uint8_t *msg, size_t pad_length, size_t num_roots) {
    correct_reed_solomon_decode_with_erasures(
        (correct_reed_solomon *)decoder, encoded, encoded_length,
        erasure_locations, erasure_length, msg);
}

rs_testbench *rs_testbench_create(size_t block_length, size_t min_distance) {
    rs_testbench *testbench = sysMalloc(sizeof(rs_testbench));
    memset(testbench, 0, sizeof(rs_testbench));

    size_t message_length = block_length - min_distance;
    testbench->message_length = message_length;
    testbench->block_length = block_length;
    testbench->min_distance = min_distance;

    testbench->msg = sysMalloc(message_length * sizeof(unsigned char));
    memset(testbench->msg, 0, message_length * sizeof(unsigned char));
    testbench->encoded = sysMalloc(block_length * sizeof(uint8_t));

    testbench->indices = sysMalloc(block_length * sizeof(int));

    testbench->corrupted_encoded = sysMalloc(block_length * sizeof(uint8_t));
    testbench->erasure_locations = sysMalloc(min_distance * sizeof(uint8_t));
    testbench->recvmsg = sysMalloc(sizeof(unsigned char) * message_length);

    return testbench;
}

void rs_testbench_destroy(rs_testbench *testbench) {
    sysFreeMem(testbench->msg);
    sysFreeMem(testbench->encoded);
    sysFreeMem(testbench->indices);
    sysFreeMem(testbench->corrupted_encoded);
    sysFreeMem(testbench->erasure_locations);
    sysFreeMem(testbench->recvmsg);
    sysFreeMem(testbench);
}

rs_test_run test_rs_errors(rs_test *test, rs_testbench *testbench, size_t msg_length,
                    size_t num_errors, size_t num_erasures) {
    rs_test_run run;
    run.output_matches = false;

    if (msg_length > testbench->message_length) {
        return run;
    }

    for (size_t i = 0; i < msg_length; i++) {
        testbench->msg[i] = rand() % 256;
    }

    size_t block_length = msg_length + testbench->min_distance;
    size_t pad_length = testbench->message_length - msg_length;

    test->encode(test->encoder, testbench->msg, msg_length, testbench->encoded);

    memcpy(testbench->corrupted_encoded, testbench->encoded, block_length);

    for (int i = 0; i < block_length; i++) {
        testbench->indices[i] = i;
    }

    shuffle(testbench->indices, block_length);

    for (unsigned int i = 0; i < num_erasures; i++) {
        int index = testbench->indices[i];
        uint8_t corruption_mask = (rand() % 255) + 1;
        testbench->corrupted_encoded[index] ^= corruption_mask;
        testbench->erasure_locations[i] = index;
    }

    for (unsigned int i = 0; i < num_errors; i++) {
        int index = testbench->indices[i + num_erasures];
        uint8_t corruption_mask = (rand() % 255) + 1;
        testbench->corrupted_encoded[index] ^= corruption_mask;
    }

    test->decode(test->decoder, testbench->corrupted_encoded, block_length,
                 testbench->erasure_locations, num_erasures,
                 testbench->recvmsg, pad_length, testbench->min_distance);

    run.output_matches = (bool)(memcmp(testbench->msg, testbench->recvmsg, msg_length) == 0);

    return run;
}
