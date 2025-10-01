#include "fec.h"
#include "obc_logging.h"
#include "obc_print.h"

void testRs(void) {
  initRs();

  packed_telem_packet_t telemData = {.data = {'-'}};

  char *testData = "Hello world!";
  for (int i = 0; i < 13; i++) {
    telemData.data[i] = (uint8_t)testData[i];
  }

  sciPrintf("Original data: %s\r\n", telemData.data);

  packed_rs_packet_t rsData = {.data = {0}};

  rsEncode(&telemData, &rsData);
  sciPrintf("Encoded data: %s\r\n", rsData.data);
  rsData.data[0] = 'a';  // Simulate a bit flip
  sciPrintf("Corrupted data: %s\r\n", rsData.data);

  uint8_t decodedData[RS_DECODED_SIZE] = {0};
  rsDecode(&rsData, decodedData, (uint8_t)RS_ENCODED_SIZE);
  sciPrintf("Decoded data: %s\r\n", decodedData);

  destroyRs();
}
