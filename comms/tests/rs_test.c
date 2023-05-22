#include "fec.h"
#include "obc_logging.h"

void testRs(void) {
    initRs();

    packed_telem_packet_t telemData = { .data = {'-'} };

    char *testData = "Hello world!";
    for (int i = 0; i < 13; i++) {
        telemData.data[i] = (uint8_t) testData[i];
    }

    LOG_DEBUG("Original data: %s", telemData.data);

    packed_rs_packet_t rsData = { .data = {0} };

    rsEncode(&telemData, &rsData);
    LOG_DEBUG("Encoded data: %s", rsData.data);

    uint8_t decodedData[REED_SOLOMON_DECODED_BYTES] = {0};
    rsDecode(&rsData, decodedData, (uint8_t) REED_SOLOMON_ENCODED_BYTES);
    LOG_DEBUG("Decoded data: %s", decodedData);

    destroyRs();
}
